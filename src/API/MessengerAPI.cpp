#define _CRT_SECURE_NO_WARNINGS
#include "MessengerAPI.h"
#include <stdio.h>

#include <boost\thread\mutex.hpp>
#include <boost\asio.hpp>
#include <boost\thread.hpp>
#include <boost\array.hpp>


#include <Protocol\TransferredData.h>
#include <Protocol\LoginRequest.h>
#include <Protocol\LoginResponse.h>
#include <Protocol\Types.h>
#include <Protocol\UserInfo.h>

#include "Query.h"

using namespace boost::asio;
using EvetQuery = typename Query<TransferredData*>;

//to hide boost headers from main include file
struct APIData
{
	EvetQuery _Query;

	io_service service;
	ip::tcp::endpoint ep;
	ip::tcp::socket sock{ service };
	boost::thread ThreadWorker;
	uint32_t ID;
	boost::array <Byte, 512> Buff;
	//acces to logined user info
	boost::mutex UserDataMutex;
};

MessengerAPI::MessengerAPI()
{
	try {
		_Data = new APIData;
	}
	catch (...)
	{
		throw;
	}
}


MessengerAPI::~MessengerAPI()
{
}

void MessengerAPI::_NewEvent(const boost::system::error_code& err_code, size_t bytes)
{
	_Data->sock.async_read_some(buffer(_Data->Buff), boost::bind(&MessengerAPI::_NewEvent, this, _1, _2));
//	printf("Recived %zd bytes\n", bytes);
	//to queue here
	if (!BaseHeader::MinimumCheck(bytes))
		return;

	switch (BaseHeader::BufferType(_Data->Buff.c_array()))
	{
	
	
	case Types::LoginResponse:
	{
		printf("LoginResponse\n");
		LoginResponse* Data = new LoginResponse;
		uint32_t err = Data->FromBuffer(_Data->Buff.c_array(),bytes);
		if (err)
		{
			printf("Error when unpack\n");
			return;
		}
		//if succes, save id
		if (Data->GetValue() == LoginResponse::Success)
		{
			boost::mutex::scoped_lock l{ _Data->UserDataMutex };
			_UserID = Data->GetId();
		}
		_Data->_Query.push_back(Data);
	}break;



	case Types::UserInfo:
	{
		printf("UserInfo\n");

		UserInfo* Data = new UserInfo;
		uint32_t err = Data->FromBuffer(_Data->Buff.c_array(), bytes);
		if (err)
		{
			printf("Error when unpack\n");
			return;
		}
		//save login if its user id
		if (Data->GetId() == _UserID)
		{
			boost::mutex::scoped_lock l{ _Data->UserDataMutex };
			_UserLogin = Data->GetLogin();
		}

		_Data->_Query.push_back(Data);
	}break;
	
	
	default:
		printf(" Wrong packet type\n");
	}

}
void MessengerAPI::_WriteHandler(const boost::system::error_code& err_code, size_t bytes)
{
	printf("Writed %zd bytes\n", bytes);
}


std::string MessengerAPI::GetCurrentUserLogin() const
{
	boost::mutex::scoped_lock l{ _Data->UserDataMutex };

	return _UserLogin; 
}
ID_t MessengerAPI::GetCurrentUserID() const
{
	boost::mutex::scoped_lock l{ _Data->UserDataMutex };

	return _UserID; 
}



int MessengerAPI::Connect(const std::string& Address, unsigned short port)
{
	_Data->ep = ip::tcp::endpoint{ ip::address::from_string(Address), port };

	boost::system::error_code err;
	_Data->sock.connect(_Data->ep, err);
	if (!err)
	{
		_Working = true;
		
		_Data->sock.async_read_some(buffer(_Data->Buff), boost::bind(&MessengerAPI::_NewEvent,this, _1, _2));
		_Data->ThreadWorker = boost::thread{ boost::bind(&boost::asio::io_service::run, &_Data->service) };
	}

	return err.value();
}

void MessengerAPI::TryLogin(const std::string& Login, const std::string& Pass)
{
	LoginRequest Request{ Login, Pass };
	Request.ToBuffer(_Data->Buff.c_array());
	_Data->sock.async_write_some(boost::asio::buffer(_Data->Buff, Request.NeededSize()),
						  boost::bind(&MessengerAPI::_WriteHandler, this, _1, _2));
}

bool MessengerAPI::TryGetMessage() {
	if (_Data->_Query.ready())
	{
		TransferredData*  data = _Data->_Query.pop_front();
		_Callback(data->GetType(), data);
		return true;
	}
	return false;
}