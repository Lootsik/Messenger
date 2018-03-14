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
	TransferredData* Data;
	if (Types::FromBuffer(connection->Packet(), connection->BytesToRead(), Data) !=
														SerializationError::Ok)
	{
#if _LOGGING_ 
		Log(Mistake, "[%s] - Wrong packet. Size %Iu bytes",
			connection->ConnectionString().c_str(), connection->BytesToRead());
#endif

		return;
	}


	if (connection->Account().Online())
	{
		_AutorizededProcess(connection, Data);
		delete Data;
		return;
	}

	auto ProcessFunc = UnauthorizedOperations.find(Data->GetType());

	//wrong type
	if (ProcessFunc == UnauthorizedOperations.end())
	{
		//TODO: add string to describe types of packet 
#if _LOGGING_ 
		Log(Mistake, "[%s] Wrong packet type forom unauthorized packet type #",
							connection->ConnectionString().c_str());
#endif

		return;
	}

	ProcessFunc->second(this, connection, Data);

	delete Data;
}


//find and call function to process 
void MessengerEngine::_AutorizededProcess(PConnection& connection, TransferredData* Data)
{
	auto ProcessFunc = AuthorizedOperations.find(Data->GetType());

	//wrong type
	if (ProcessFunc == AuthorizedOperations.end())
	{
		//TODO: add string to describe types
		Log(Mistake, "[%s] Wrong packet type forom authorized packet type #",
					connection->ConnectionString().c_str());

		return;
	}

	ProcessFunc->second(this, connection,Data);
}


/*
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						Request Process
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
	

void MessengerEngine::OnLogin(PConnection& connection, TransferredData* Request)
{
	LoginResponse Response = _AccountManager.Login(*static_cast<LoginRequest*>(Request));

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

void MessengerEngine::OnLogout(PConnection& connection, TransferredData* Packet)
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

bool MessengerEngine::_MakePacket(PConnection& connection, const TransferredData& Item)
{
	uint32_t err = Item.ToBuffer(connection->WriteBuffer().c_array());
	if (err)
		return false;

	connection->SetBytesToWrite(Item.NeededSize());
	return true;
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

void MessengerEngine::OnUserInfo(PConnection& connection, TransferredData* User)
{
	std::string login = _AccountManager.FindLogin(static_cast<UserInfo*>(User)->GetId());
	//there is no user with such id
	//if (login == "")
		//return;

	UserInfo ReturnInfo{ static_cast<UserInfo*>(User)->GetId(), login };

	SendResponce(connection, ReturnInfo);
}


void MessengerEngine::OnMessage(PConnection& connection, TransferredData* mess)
{
	auto message = static_cast<Message*>(mess);
	if (message->Sender() != connection->Account().ID())
	{
		Log(Mistake, "Sended message not from %s", connection->ConnectionString().c_str());
	}

	_MessageManager.PostMessageUser(message->Sender(), message->Receiver(), message->Content());
	auto NewMess =_MessageManager.GetMessageUser(message->Sender(), message->Receiver(), 
						_MessageManager.GetLastMessageID(message->Sender(), message->Receiver(), message->Sender()).GetMessageId());
	// send to sender
	SendResponce(connection, NewMess);

	auto User = _Server->FindConnected(message->Receiver());
	if (User == nullptr)
		return;

	SendResponce(*User, NewMess);

}

// parse packet 
void MessengerEngine::OnMessageRequest(PConnection& connection, TransferredData* request)
{
	auto Request = static_cast<MessageRequest*>(request);
	//TODO: divide message request 
	switch (Request->GetRequestType())
	{
	case MessageRequest::Message:
	{
		OnMessageRequest_M(connection, *Request);
	}break;

	case MessageRequest::LastMessageID:
	{
		OnLastMessage(connection, *Request);
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


void MessengerEngine::ForceLogout(PConnection& connection)
{
	OnLogout(connection,nullptr); 
}