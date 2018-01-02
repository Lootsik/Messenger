#include <vector>
#include "MessengerEngine.h"
#include "Server.h"
#include "..\PacketFormat\Deserialization.h"
#include "..\PacketFormat\Serialization.h"
#include "Logger.h"

using namespace Logger;

static void BindClientToAcc(Client* client, Account* account)
{
	client->_Account = account;
	account->_Client = client;

	client->_LoggedIn = true;
	account->_Online = true;
}

static bool PreBindChecks(Client* client, Account* account)
{
	return  !(client->_LoggedIn || account->_Online);
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
	return !(!_DB.Connect("tcp://127.0.0.1:3306", "root", "11JustLikeYou11") ||
				!_DB.CreatePrepared("messeger_server_db", "users"));
}

//TODO: rewrite this
void MessengerEngine::FillAccountInfo()
{
	auto Logins = _DB.FillLogins();
	//TODO: передалать это
	size_t LoginsSize = Logins.size();
	//поле уникальное, но желательно проверить, так как бд будут меняться
	//TODO: сделать hash table для этого случая
	for (size_t i = 0; i < LoginsSize; ++i) {
		_Accounts[Logins[i].first] = new Account{ Logins[i].first, std::move(Logins[i].second) };
	}
}


//TODO: пересмотреть причины и сделать ответы
void MessengerEngine::Login(Client* client, const std::string& entered_login, const std::string& entered_password)
{
	//TODO: this can only check, that pass is apply to login
	//we can take more info forom this process
	uint32_t FindId = CheckAccount(entered_login, entered_password);

	int Result = (int)Packet::LoginResult::Result::Wrong;
	int Id = 0;
		
	if (FindId != INVALID_ID) {
		Account* Acc = _Accounts[FindId];

		if (PreBindChecks(client, Acc)) 
		{
			BindClientToAcc(client, Acc);

			Result = (int)Packet::LoginResult::Result::Success;
			Id = FindId;
		}
	}
	//need to hide this
	Serialization::MakePacketLoginResult(client->_WriteBuf.c_array(), Result, FindId);

#if _STATE_MESSAGE_
	if (Result == (int)Packet::LoginResult::Result::Success)
		LogBoth(Action, "[%s] - Logged in", ClientString(client).c_str());
	else
		Log(Mistake, "[%s] - Failed login", ClientString(client).c_str());

#endif 

	//send result
	Response(client); 
}


void MessengerEngine::Logout(Client* client)
{
#if _STATE_MESSAGE_
	LogBoth(Action, "[%s] - Logout", ClientString(client).c_str());
#endif // _STATE_MESSAGE_

	//возможно, когда-то стоит изменить последовательность
	client->_Account->_Client = nullptr;
	client->_Account->_Online = false;
	client->_Account = nullptr;

	client->_LoggedIn = false;
}




uint32_t MessengerEngine::CheckAccount(const std::string& entered_login, const std::string& entered_password)
{
	size_t id = _DB.FetchUser(entered_login, entered_password);
	//fix this
	if (id > UINT32_MAX)
		throw;

	return static_cast<uint32_t>(id);
}

void MessengerEngine::AnalyzePacket(Client* client, size_t size)
{
	if (!Deserialization::PacketCheckup(client->_ReadBuff.c_array(), size)){
		//TODO: log
#if _STATE_MESSAGE_ && _PACKET_TRACE_
		Log(Action, "[%s] - Packet %Iu bytes", ClientString(client).c_str(), size);
#endif
		return;
	}
	//not logined already
	if (Deserialization::PaketType(client->_ReadBuff.c_array(), size) != (int)Packet::Types::Login && !client->_LoggedIn)
		return;

	//TODO: divide this
	switch (Deserialization::PaketType(client->_ReadBuff.c_array(), size))
	{
	case (int)Packet::Types::Login: 
	{
		OnLogin(client);
	}break;

	case (int)Packet::Types::Logout:
	{
		//...
		Deserialization::OnLogout();
		Logout(client);
	}break;

	case (int)Packet::Types::Message:
	{
		OnMessage(client);
	}break;

	default:
		break;
	}
}

//calculate size of message and send it
void MessengerEngine::Response(Client* client)
{
	client->BytesWrite = Serialization::CountSize(client->_WriteBuf.c_array());
	_Server->Send(client);
}

void MessengerEngine::OnLogin(Client* client)
{
	std::string GuessLogin;
	std::string GuessPassword;
	int res = Deserialization::OnLogin(client->_ReadBuff.c_array(), client->BytesRead,
										GuessLogin, GuessPassword);

	if (res == (int)Deserialization::Result::Ok)
		//TODO: do smth with result
		Login(client, GuessLogin, GuessPassword);
}

//TODO: notification, that message reached the addressee
void MessengerEngine::OnMessage(Client* client)
{
	uint32_t from, to, mess_size;
	char* Message = NULL;

	int res = Deserialization::OnMessage(client->_ReadBuff.c_array(),
										 client->BytesRead,
										 from, to, mess_size, &Message);

	if (from != client->_Account->ID)
	{
#if _STATE_MESSAGE_
		Log(Action, "[%s] - Message wrong addresser ID: %Iu", ClientString(client).c_str(), from);
#endif
		return;
	}

	//TODO: offline message
	if (res == (int)Deserialization::Result::Ok)
	{
		if (_Accounts.find(to) == _Accounts.end())
		{
#if _STATE_MESSAGE_
			Log(Action, "[%s] - Message wrong addresat ID: %Iu", ClientString(client).c_str(), to);
#endif
			return;
		}
		auto Acc = _Accounts[to];
		if (Acc->_Online) {
			memcpy(Acc->_Client->_WriteBuf.c_array(), client->_ReadBuff.c_array(), client->BytesRead);
#if _STATE_MESSAGE_
			Log(Action, "[%s] message %d bytes to ID: %Iu",ClientString(client).c_str(),client->BytesRead, to);
#endif 			
			Response(Acc->_Client);
		}
	}
}