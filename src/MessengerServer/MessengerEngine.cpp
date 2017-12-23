#include <vector>
#include "MessengerEngine.h"
#include "Server.h"
#include "..\PacketFormat\Deserialization.h"
#include "..\PacketFormat\Serialization.h"

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

MessengerEngine::MessengerEngine(Server* server)
//считывать значения с конфигов
	:_Server{ server }
{
	bool res = _DB.Connect("tcp://127.0.0.1:3306", "root", "11JustLikeYou11", "messeger_server_db", "users");
	
	//TODO: сделать с дб
	auto Logins = _DB.FillLogins();
	//TODO: передалать это
	size_t LoginsSize = Logins.size();
	//поле уникальное, но желательно проверить, так как бд будут меняться
	//TODO: сделать hash table для этого случая
	for (size_t i = 0; i < LoginsSize; ++i) {
		_Accounts[Logins[i].first] = new Account{ Logins[i].first, std::move(Logins[i].second) };
	}
}


MessengerEngine::~MessengerEngine()
{
}

//TODO: пересмотреть причины и сделать ответы
void MessengerEngine::Login(Client* client, const std::string& entered_login, const std::string& entered_password)
{
	//TODO: this can only check, that pass is apply to login
	//we can take more info forom this process
	uint32_t FindId = CheckAccount(entered_login, entered_password);

	int Result = (int)LoginResult::Result::Wrong;
	int Id = 0;
		
	if (FindId != INVALID_ID) {
		Account* Acc = _Accounts[FindId];

		if (PreBindChecks(client, Acc)) 
		{
			BindClientToAcc(client, Acc);

			Result = (int)LoginResult::Result::Success;
			Id = FindId;
		}
	}
			
	//need to hide this
	Serialization::MakePacketLoginResult(client->_WriteBuf.c_array(), Result, FindId);
	//send result
	Response(client); 
}


void MessengerEngine::Logout(Client* client)
{
#ifdef _STATE_MESSAGE_
	std::cout << client->_Account->_Login << "- logged out\n";
#endif // _STATE_MESSAGE_

	//возможно, когда-то стоит изменить последовательность
	client->_Account->_Client = nullptr;
	client->_Account->_Online = false;
	client->_Account = nullptr;

	client->_LoggedIn = false;
}




uint32_t MessengerEngine::CheckAccount(const std::string& entered_login, const std::string& entered_password)
{
	size_t id =  _DB.FetchUser(entered_login, entered_password);
	//fix this
	if (id > UINT32_MAX)
		throw;

	return id;
}

void MessengerEngine::AnalyzePacket(Client* client, size_t size)
{
	if (!Deserialization::PacketCheckup(client->_ReadBuff.c_array(), size)){
		//TODO: log
#ifdef _STATE_MESSAGE_
		std::cout << "Packet from " << *client << " failed\n";
#endif
		return;
	}
	//not logined already
	if (Deserialization::PaketType(client->_ReadBuff.c_array(), size) != (int)PacketTypes::Login && !client->_LoggedIn)
		return;

	//TODO: divide this
	switch (Deserialization::PaketType(client->_ReadBuff.c_array(), size))
	{
	case (int)PacketTypes::Login: 
	{
		OnLogin(client);
	}break;

	case (int)PacketTypes::Logout:
	{
		//...
		Deserialization::OnLogout();
		Logout(client);
	}break;

	case (int)PacketTypes::Message:
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
#ifdef _STATE_MESSAGE_
		std::cout << *client << ": Wrong id in message: " << from << "\n";
#endif
		return;
	}

	//TODO: offline message
	if (res == (int)Deserialization::Result::Ok)
	{
		if (_Accounts.find(to) == _Accounts.end())
		{
#ifdef _STATE_MESSAGE_
			std::cout << *client << ": Message to ID: " << to << "\nError ID not found\n";
#endif
			return;
		}
		auto Acc = _Accounts[to];
		if (Acc->_Online) {
			memcpy(Acc->_Client->_WriteBuf.c_array(), client->_ReadBuff.c_array(), client->BytesRead);
			
			Response(Acc->_Client);
		}
	}
}