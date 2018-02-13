#define _CRT_SECURE_NO_WARNINGS
#include "MessengerAPI.h"
#include <stdio.h>

#include <Protocol\TransferredData.h>
#include <Protocol\LoginRequest.h>
#include <Protocol\LoginResponse.h>
#include <Protocol\Types.h>

MessengerAPI::MessengerAPI()
{
}


MessengerAPI::~MessengerAPI()
{
}

void MessengerAPI::_NewEvent(const boost::system::error_code& err_code, size_t bytes)
{
	printf("Recived %zd bytes\n", bytes);
	//to queue here
	if (!BaseHeader::MinimumCheck(bytes))
		return;

	TransferredData* data = nullptr;

	switch (BaseHeader::BufferType(Buff.c_array()))
	{
	case Types::LoginResponse:
	{
		TransferredData* Data = new LoginResponse;
		uint32_t err = Data->FromBuffer(Buff.c_array(),bytes);
		if (err)
		{
			printf("Error when unpack\n");
			return;
		}

		_Query.push_back(Data);
	}break;
	default:
		printf(" Wrong packet type\n");
	}

}
void MessengerAPI::_WriteHandler(const boost::system::error_code& err_code, size_t bytes)
{
	printf("Writed %zd bytes\n", bytes);
}


int MessengerAPI::Connect(const std::string& Address, unsigned short port)
{
	ep = ip::tcp::endpoint{ ip::address::from_string(Address), port };

	boost::system::error_code err;
	sock.connect(ep, err);
	if (!err)
	{
		_Working = true;
		
		sock.async_read_some(buffer(Buff), boost::bind(&MessengerAPI::_NewEvent,this, _1, _2));
		ThreadWorker = boost::thread{ boost::bind(&boost::asio::io_service::run, &service) };
	}

	return err.value();
}

void MessengerAPI::TryLogin(const std::string& Login, const std::string& Pass)
{
	LoginRequest Request{ Login, Pass };
	Request.ToBuffer(Buff.c_array());
	sock.async_write_some(boost::asio::buffer(Buff, Request.NeededSize()), 
						  boost::bind(&MessengerAPI::_WriteHandler, this, _1, _2));
}