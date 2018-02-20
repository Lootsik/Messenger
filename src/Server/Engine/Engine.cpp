#include <vector>
#include <Network\Network.h>
#include <Protocol\BaseHeader.h>
#include <Protocol\LoginRequest.h>
#include <Protocol\LoginResponse.h>
#include <Protocol\UserInfo.h>

#include <Protocol\Types.h>
#include <Protocol\SerializationError.h>
//#include <PacketFormat\PacketFormat.h>
#include <Logger\Logger.h>
#include "Engine.h"

using namespace Logger;

static void LogLogin(const PConnection& connection, const LoginResponse& Result)
{
	if(Result.GetValue() == LoginResponse::Success)
		LogBoth(Action, "[%s] - Logged in", connection->ConnectionString().c_str());
	else 
		Log(Mistake, "[%s] - Failed login", connection->ConnectionString().c_str());
}

//TODO: rewrite this
MessengerEngine::MessengerEngine(Network* server)
//считывать значения с конфигов
	:_Server{ server }
{
}

/*
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
void MessengerEngine::AnalyzePacket(PConnection connection)
{
	
	if (!BaseHeader::MinimumCheck(connection->BytesToRead())){
#if _LOGGING_ 
		Log(Mistake, "[%s] - Packet wrong marking. Size %Iu bytes",
						connection->ConnectionString().c_str(), connection->BytesToRead());
#endif
		return;
	}

	//TODO: divide this
	switch (BaseHeader::BufferType(connection->ReadBuffer().c_array()))
	{

	case Types::LoginRequest:
	{
		OnLogin(connection);
	}break;

	case Types::Logout:
	{
		OnLogout(connection);
	}break;

	case Types::UserInfo:
	{
		OnUserInfo(connection);
	}
	/*case (int)Packet::Types::Message:
	{
		//OnMessage(connection);
	}break;*/

	default:
		break;
	}
}

void MessengerEngine::OnLogin(PConnection& connection)
{
	LoginRequest Request;

	uint32_t err = Request.FromBuffer(connection->ReadBuffer().c_array(), connection->BytesToRead());
	if(err){
		//TODO: do smth with result
		return;
	}

	LoginResponse Response = _AccountManager.Login(Request);

#if _LOGGING_
	LogLogin(connection, Response);
#endif
	SendResponce(connection, Response);


	if (Response.GetValue() == LoginResponse::Success)
	{
		connection->Account().SetID( Response.GetId());
		
		//if success send packet with login
		UserInfo ReturnInfo{ Response.GetId(), 
					_AccountManager.FindLogin(Response.GetId())};

		SendResponce(connection, ReturnInfo);
	}

}

void MessengerEngine::OnLogout(PConnection& connection)
{
	_AccountManager.Logout(connection->Account().ID());
	connection->Account().Reset();
#if _LOGGING_
	LogBoth(Action, "[%s] - Logout", connection->ConnectionString().c_str());
#endif // _LOGGING_
}

//TODO: rewrite according to changes
//calculate size of message and _Send it
void MessengerEngine::_Send(PConnection& Connection)
{
	_Server->Send(Connection);
}

void MessengerEngine::SendResponce(PConnection& connection, const TransferredData& Result)
{
	if (!_MakePacket(connection, Result))
	{
		Log(Mistake, "Packet not cannot be sended");
		return;
	}
	//_Send result
	_Send(connection);
}

void MessengerEngine::OnUserInfo(PConnection& connection)
{
	UserInfo Info;

	uint32_t err = Info.FromBuffer(connection->ReadBuffer().c_array(), connection->BytesToRead());
	if (err) {
		//TODO: do smth with result
		return;
	}

	std::string login = _AccountManager.FindLogin(Info.GetId());
	//there is no user with such id
	//if (login == "")
		//return;

	UserInfo ReturnInfo{ Info.GetId(), login };

	SendResponce(connection, ReturnInfo);
}