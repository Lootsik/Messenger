#define _CRT_SECURE_NO_WARNINGS
#include "MessengerAPI.h"
#include <stdio.h>

#include <boost\thread\mutex.hpp>
#include <boost\asio.hpp>
#include <boost\thread.hpp>
#include <boost\array.hpp>

#include <API\AsyncStrategy.h>
#include <API\NetworkStrategy.h>
#include <API\PacketAnalyzer.h>

#include <Protocol\Types.h>

using namespace boost::asio;


//to hide boost headers from main include file
struct APIData
{
	boost::mutex UserDataMutex;
	uint32_t ID;
	//acces to logined user info
	NetworkStrategy* Network;
};

MessengerAPI::MessengerAPI()
{
	try {
		_Data = new APIData;
		_Data->Network = new AsyncStrategy(PacketAnalyzer{},*this);
	}
	catch (...)
	{
		throw;
	}
}


MessengerAPI::~MessengerAPI()
{
	delete _Data->Network;
	delete _Data;
}

// if user need to see that packet, return false
bool MessengerAPI::Process(const TransferredData* data)
{
	static bool UserInfoAfterLogin = false;
	switch (data->GetType())
	{

	case Types::LoginResponse:
	{
		const LoginResponse* Data = static_cast<const LoginResponse*>(data);
		//if succes, save id
		if (Data->GetValue() == LoginResponse::Success)
		{
			_SetUserId(Data->GetId());
			UserInfoAfterLogin = true;
		}

		return false;
	}break;


	case Types::UserInfo:
	{
		const UserInfo* Data = static_cast<const UserInfo*>(data);
		if (Data->GetId() == _UserID && UserInfoAfterLogin == true)
		{
			boost::mutex::scoped_lock l{ _Data->UserDataMutex };
			_UserLogin = Data->GetLogin();
			UserInfoAfterLogin = false;
			return true;
		}
		return false;
	}break;


	case Types::Message:
	{
		//const Message* Data = static_cast<const Message*>(data);

		return false;
	}
	case Types::LastMessageResponse:
	{
		//const LastMessageResponse* Data = static_cast<const LastMessageResponse*>(data);

		return false;
	}

	
	}
	return true;
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

void MessengerAPI::_SetUserId(const ID_t id)
{
	boost::mutex::scoped_lock l{ _Data->UserDataMutex };
	_UserID = id;
}
void MessengerAPI::_SetUserLogin(const std::string& NewLogin)
{
	boost::mutex::scoped_lock l{ _Data->UserDataMutex };
	_UserLogin = NewLogin;
}



bool MessengerAPI::Connect(const std::string& Address, unsigned short port) 
{
	try
	{
		return _Data->Network->Connect( Address, port );
	}
	catch (const NetworkStrategy::__ConnectionRefused&)
	{
		throw ConnectionRefused{};
	}
}

int MessengerAPI::TryLogin(const std::string& Login, const std::string& Pass)
{
	LoginRequest Request{ Login, Pass };
	_Data->Network->Send(Request);
	try {
		for (int i = 0; i < 15; ++i)
		{
			try {
				if (Ready())
				{
					auto ptrPacket = GetPacket();
					const TransferredData* Packet = ptrPacket.get();
					if (Packet->GetType() == Types::LoginResponse)
					{
						const LoginResponse* Response = (const LoginResponse*)Packet;

						_Authorized = true;

						return Response->GetValue();
					}
					break;
				}
				Sleep(200);
			}
			catch (const Disconnect&)
			{
				return TryLogin(Login, Pass);
			}
		}
	}
	catch (const Disconnect&)
	{
		throw;
	}
	//TODO: add 
	return -1;
}

bool MessengerAPI::Ready() 
{
	try {
		return _Data->Network->Ready();
	}
	catch (const NetworkStrategy::__ConnectionDropped&)
	{
		throw Disconnect{};
	}
}



std::shared_ptr<BaseHeader> MessengerAPI::GetPacket()
{
	return std::shared_ptr<BaseHeader>{ _Data->Network->GetPacket() };
}

void MessengerAPI::Quit()
{
	if (_Authorized)
	{
		Logout packet;
		_Data->Network->Send(packet);

		_Authorized = false;
	}	
	_Data->Network->Release();

	_SetUserId(INVALID_ID);
	_SetUserLogin("");

}


void MessengerAPI::GetUserLogin(ID_t Id)
{
	UserInfo info{ Id };
	_Data->Network->Send(info);
}
void MessengerAPI::LastMessageId(ID_t another)
{
	MessageRequest Requset{ MessageRequest::LastMessageID, GetCurrentUserID(), another, };
	_Data->Network->Send(Requset);
}
void MessengerAPI::LoadMessage(ID_t another, uint32_t index)
{
	MessageRequest Requset{ MessageRequest::Message, GetCurrentUserID(), another, index };
	_Data->Network->Send(Requset);
}
void MessengerAPI::SendMessageTo(ID_t to, const std::wstring& Content)
{
	Message message{ GetCurrentUserID(), to, Content };
	_Data->Network->Send(message);
}