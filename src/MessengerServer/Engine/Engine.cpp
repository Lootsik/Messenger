#include <vector>
#include <Server\Server.h>
#include <Protocol\BaseHeader.h>
#include <Protocol\LoginRequest.h>
#include <Protocol\LoginResponse.h>
#include <Protocol\Types.h>
#include <Protocol\SerializationError.h>
//#include <PacketFormat\PacketFormat.h>
#include <Logger\Logger.h>
#include "Engine.h"

using namespace Logger;

static void LogLogin(const Connection* connection, const LoginResponse& Result)
{
	if(Result.GetValue() == LoginResponse::Success)
		LogBoth(Action, "[%s] - Logged in", ConnectionString(connection).c_str());
	else 
		Log(Mistake, "[%s] - Failed login", ConnectionString(connection).c_str());
}

//TODO: rewrite this
MessengerEngine::MessengerEngine(Server* server)
//считывать значения с конфигов
	:_Server{ server }
{
}

//TODO: smth with db initialization
//TODO: add actually load from config this
bool MessengerEngine::LoadFromConfig(const char* Filename)
{
	return true;
}


/*
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
void MessengerEngine::AnalyzePacket(Connection* connection)
{
	
	if (!BaseHeader::MinimumCheck(connection->BytesRead)){
#if _LOGGING_ 
		Log(Mistake, "[%s] - Packet wrong marking. Size %Iu bytes",
					ConnectionString(connection).c_str(), connection->BytesRead);
#endif
		return;
	}

	//TODO: divide this
	switch (BaseHeader::BufferType( connection->ReadBuf()) )
	{

	case Types::LoginRequest:
	{
		OnLogin(connection);
	}break;

	case Types::Logout:
	{
		OnLogout(connection);
	}break;

	/*case (int)Packet::Types::Message:
	{
		//OnMessage(connection);
	}break;*/

	default:
		break;
	}
}

void MessengerEngine::OnLogin(Connection* connection)
{
	LoginRequest Request;

	uint32_t err = Request.FromBuffer(connection->ReadBuf(), connection->BytesRead);
	if(err){
		//TODO: do smth with result
		return;
	}

	LoginResponse Response = _AccountManager.Login(Request);
	//TODO: CHANHE THIS ZERO
	if (Response.GetValue() == LoginResponse::Success)
	{
		connection->Account.ID = Response.GetId();
	}
#if _LOGGING_
	LogLogin(connection, Response);
#endif
	SendLoginResponce(connection, Response);
}

void MessengerEngine::OnLogout(Connection* connection)
{
	_AccountManager.Logout(connection->Account.ID);
#if _LOGGING_
	LogBoth(Action, "[%s] - Logout", ConnectionString(connection).c_str());
#endif // _LOGGING_
}

//TODO: rewrite according to changes
//calculate size of message and _Send it
void MessengerEngine::_Send(Connection* Connection)
{
	_Server->Send(Connection);
}

void MessengerEngine::SendLoginResponce(Connection* connection, const LoginResponse& Result)
{
	if (!_MakePacket(connection, Result))
	{
		Log(Mistake, "Packet not cannot be sended");
		return;
	}
	//_Send result
	_Send(connection);
}