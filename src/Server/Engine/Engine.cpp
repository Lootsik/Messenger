#include <vector>
#include <Network\Network.h>
#include <Protocol\BaseHeader.h>
#include <Protocol\LoginRequest.h>
#include <Protocol\LoginResponse.h>
#include <Protocol\UserInfo.h>
#include <Protocol\MessageRequest.h>
#include <Protocol\Message.h>
#include <Protocol\LastMessageResponse.h>


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
	:_Server{ server }, _MessageManager{ _AccountManager }
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

	if (connection->Account().Online())
	{
		_AutorizededProcess(connection);
		return;
	}

	//TODO: divide this
	switch (BaseHeader::BufferType(connection->ReadBuffer().c_array()))
	{

	case Types::LoginRequest:
	{
		OnLogin(connection);
	}break;

	default:
		break;
	}
}



void MessengerEngine::_AutorizededProcess(PConnection& connection)
{
	switch (BaseHeader::BufferType(connection->ReadBuffer().c_array()))
	{

	case Types::Logout:
	{
		OnLogout(connection);
	}break;

	case Types::UserInfo:
	{
		OnUserInfo(connection);
	}break;

	//acces to specific message
	case Types::MessageRequest:
	{
		OnMessageRequest(connection);
	}break;

	case Types::Message:
	{
		OnMessage(connection);
	}break;

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


void MessengerEngine::OnMessage(PConnection& connection)
{
	Message message;
	int err = message.FromBuffer(connection->ReadBuffer().c_array(),
						connection->BytesToRead());
	if (err) {
		return;
	}

	if (message.Sender() != connection->Account().ID())
	{
		Log(Mistake, "Sended message not from %s", connection->ConnectionString().c_str());
	}

	_MessageManager.PostMessageUser(message.Sender(), message.Receiver(), message.Content());

	//TODO: add response maybe 
}

// parse packet 
void MessengerEngine::OnMessageRequest(PConnection& connection)
{
	MessageRequest Request;
	int err = Request.FromBuffer(connection->ReadBuffer().c_array(),
							connection->BytesToRead());
	if (err) {
		return;
	}

	switch (Request.GetRequestType())
	{
	case MessageRequest::Message:
	{

	}break;

	case MessageRequest::LastMessageID:
	{

	}break;

	default:

#if _LOGGING_
		Log(Mistake, "[%s] - Wrong MessageRequest type", connection->ConnectionString().c_str());
#endif // _LOGGING_
		break;
	}


}

	// INVALID_ID sender if error
void MessengerEngine::OnMessageRequest(PConnection& connection, MessageRequest& Request)
{
	auto Response = _MessageManager.GetMessageUser(Request.Sender(),Request.Receiver(),Request.GetMessageIndex());
	SendResponce(connection, Response);
}

void MessengerEngine::OnLastMessage(PConnection& connection, MessageRequest& Request)
{
	auto Response = _MessageManager.GetLastMessageID(Request.Sender(),Request.Receiver());
	SendResponce(connection, Response);
}