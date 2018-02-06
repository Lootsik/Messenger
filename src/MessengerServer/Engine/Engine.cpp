#include <vector>
#include <Server\Server.h>
#include <PacketFormat\PacketFormat.h>
#include <Accounts\AuthenticationData.h>
#include <Logger\Logger.h>
#include "Engine.h"

using namespace Logger;

static void LogLogin(const Connection* connection, const AuthenticationResult& Result)
{
	if(Result.GetValue() == (int)Packet::LoginResult::Result::Success)
		LogBoth(Action, "[%s] - Logged in", ConnectionString(connection).c_str());
	else 
		Log(Mistake, "[%s] - Failed login", ConnectionString(connection).c_str());
}

//TODO: преписать это
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
	if (!SerializableTypes::Types::BasickSizeCheck(connection->BytesRead)){
#if _LOGGING_ 
		Log(Mistake, "[%s] - Packet wrong marking. Size %Iu bytes",
					ConnectionString(connection).c_str(), connection->BytesRead);
#endif
		return;
	}

	//TODO: divide this
	switch (SerializableTypes::Types::SerializedType( connection->ReadBuf()) )
	{

	case SerializableTypes::AuthenticationData:
	{
		OnLogin(connection);
	}break;

	case SerializableTypes::Logout:
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
	using namespace Packet;
	AuthenticationData AuthData;

	if (!AuthData.FromBuffer(connection->ReadBuf(), connection->BytesRead))
	{
		//TODO: do smth with result
	}
	AuthenticationResult Result = _AccountManager.Login(AuthData);
	//TODO: CHANHE THIS ZERO
	if (Result.GetValue() == Packet::LoginResult::Success)
	{
		connection->Account.ID = Result.GetId();
	}
#if _LOGGING_
	LogLogin(connection, Result);
#endif
	SendLoginResponce(connection, Result);
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
	_Server->_Send(Connection);
}

void MessengerEngine::SendLoginResponce(Connection* connection, const AuthenticationResult& Result)
{
	_MakePacket(connection, Result);
	//_Send result
	_Send(connection);
}