#include <vector>
#include <Network\Network.h>

#include <Protocol\Types.h>
#include <Protocol\SerializationError.h>
#include <Protocol\BaseHeader.h>

#include <Logger\Logger.h>

#include "Engine.h"


//TODO: make template to unpack
//TODO: add constructor from buffer to transferable types

using namespace Logger;

//TODO: remove this
static void LogLogin(const PConnection& connection, const LoginResponse& Result)
{
	if(Result.GetValue() == LoginResponse::Success)
		LogBoth(Action, "[%s] - Logged in", connection->ConnectionString().c_str());
	else 
		Log(Mistake, "[%s] - Failed login", connection->ConnectionString().c_str());
}

MessengerEngine::MessengerEngine(Network* server)
//считывать значения с конфигов
	:_Server{ server }, _MessageManager{ _AccountManager }
	, UnauthorizedOperations
	{
		{ Types::LoginRequest, &MessengerEngine::OnLogin }
	}
	, AuthorizedOperations
	{
		{ Types::Logout ,&MessengerEngine::OnLogout },
		{ Types::UserInfo , &MessengerEngine::OnUserInfo },
		{ Types::MessageRequest, &MessengerEngine::OnMessageRequest },
		{ Types::Message, &MessengerEngine::OnMessage }
	}
{
}

/*
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						Chose Packet Process
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

//basic check and call function to process unauthorized request
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

	auto ProcessFunc = UnauthorizedOperations.find(
			BaseHeader::BufferType(connection->Packet()));

	//wrong type
	if (ProcessFunc == UnauthorizedOperations.end())
	{
		//TODO: add string to describe types of packet 
		Log(Mistake, "[%s] Wrong packet type forom unauthorized packet type #",
							connection->ConnectionString().c_str());

		return;
	}

	ProcessFunc->second(this, connection);
}


//find and call function to process 
void MessengerEngine::_AutorizededProcess(PConnection& connection)
{
	auto ProcessFunc = AuthorizedOperations.find(
		BaseHeader::BufferType(connection->Packet()));

	//wrong type
	if (ProcessFunc == AuthorizedOperations.end())
	{
		//TODO: add string to describe types
		Log(Mistake, "[%s] Wrong packet type forom authorized packet type #",
					connection->ConnectionString().c_str());

		return;
	}

	ProcessFunc->second(this, connection);
}


/*
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						Request Process
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
	

void MessengerEngine::OnLogin(PConnection& connection)
{
	LoginRequest Request;

	uint32_t err = Request.FromBuffer(connection->Packet(), connection->BytesToRead());
	if(err){
		Log(Mistake, "[%s] Error when unpacking login request",
			connection->ConnectionString().c_str());

		return;
	}

	LoginResponse Response = _AccountManager.Login(Request);

	SendResponce(connection, Response);


	if (Response.GetValue() == LoginResponse::Success)
	{
		connection->Account().SetID( Response.GetId());
		
		//if success send packet with login
		UserInfo ReturnInfo{ Response.GetId(), 
					_AccountManager.FindLogin(Response.GetId())};

		SendResponce(connection, ReturnInfo);
	}

#if _LOGGING_
	LogLogin(connection, Response);
#endif
}

void MessengerEngine::OnLogout(PConnection& connection)
{
	_AccountManager.Logout(connection->Account().ID());
	connection->Account().Reset();
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
	UserInfo Users;

	uint32_t err = Users.FromBuffer(connection->Packet(), connection->BytesToRead());
	if (err) {
		Log(Mistake, "[%s] Error when unpacking User info",
			connection->ConnectionString().c_str());
		return;
	}

	std::string login = _AccountManager.FindLogin(Users.GetId());
	//there is no user with such id
	//if (login == "")
		//return;

	UserInfo ReturnInfo{ Users.GetId(), login };

	SendResponce(connection, ReturnInfo);
}


void MessengerEngine::OnMessage(PConnection& connection)
{
	Message message;
	int err = message.FromBuffer(connection->Packet(),
						connection->BytesToRead());
	if (err) {
		Log(Mistake, "[%s] Error when unpacking Mesage",
			connection->ConnectionString().c_str());

		return;
	}

	if (message.Sender() != connection->Account().ID())
	{
		Log(Mistake, "Sended message not from %s", connection->ConnectionString().c_str());
	}

	_MessageManager.PostMessageUser(message.Sender(), message.Receiver(), message.Content());
	auto NewMess =_MessageManager.GetMessageUser(message.Sender(), message.Receiver(), 
						_MessageManager.GetLastMessageID(message.Sender(), message.Receiver(), message.Sender()).GetMessageId());
	// send to sender
	SendResponce(connection, NewMess);

	auto User = _Server->FindConnected(message.Receiver());
	if (User == nullptr)
		return;

	SendResponce(*User, NewMess);

}

// parse packet 
void MessengerEngine::OnMessageRequest(PConnection& connection)
{
	MessageRequest Request;
	int err = Request.FromBuffer(connection->Packet(),
							connection->BytesToRead());
	if (err) {
		return;
	}

	//TODO: divide message request 
	switch (Request.GetRequestType())
	{
	case MessageRequest::Message:
	{
		OnMessageRequest_M(connection, Request);
	}break;

	case MessageRequest::LastMessageID:
	{
		OnLastMessage(connection, Request);
	}break;

	default:

#if _LOGGING_
		Log(Mistake, "[%s] - Wrong MessageRequest type", connection->ConnectionString().c_str());
#endif // _LOGGING_
		break;
	}


}

	// INVALID_ID sender if error
void MessengerEngine::OnMessageRequest_M(PConnection& connection, MessageRequest& Request)
{
	auto Response = _MessageManager.GetMessageUser(Request.Sender(),Request.Receiver(),Request.GetMessageIndex());
#if _LOGGING_
	Log(Action, "[%s] - Message sended", connection->ConnectionString().c_str());
#endif // _LOGGING_
	SendResponce(connection, Response);
}

void MessengerEngine::OnLastMessage(PConnection& connection, MessageRequest& Request)
{
	auto Response = _MessageManager.GetLastMessageID(Request.Sender(), Request.Receiver(), connection->Account().ID());
#if _LOGGING_
	Log(Action, "[%s] - Last message sended", connection->ConnectionString().c_str());
#endif // _LOGGING_
	SendResponce(connection, Response);
}