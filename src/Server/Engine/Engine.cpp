#include <vector>
#include <Network\Network.h>
#include <Protocol\BaseHeader.h>
#include <Protocol\LoginRequest.h>
#include <Protocol\LoginResponse.h>
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

	if (Response.GetValue() == LoginResponse::Success)
	{
		connection->Account().SetID( Response.GetId());
	}
#if _LOGGING_
	LogLogin(connection, Response);
#endif
	SendLoginResponce(connection, Response);
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

void MessengerEngine::SendLoginResponce(PConnection& connection, const LoginResponse& Result)
{
	if (!_MakePacket(connection, Result))
	{
		Log(Mistake, "Packet not cannot be sended");
		return;
	}
	//_Send result
	_Send(connection);
}