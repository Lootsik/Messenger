#include <vector>
#include "MessengerEngine.h"
#include "Server.h"
#include "..\PacketFormat\Deserialization.h"
#include "..\PacketFormat\Serialization.h"

void BindClientToAcc(Client* client, Account* account)
{
	client->_Account = account;
	account->_Client = client;

	client->_LoggedIn = true;
	account->_Online = true;
}

MessengerEngine::MessengerEngine(Server* server)
//��������� �������� � ��������
	:_Server{ server }
{
	bool res = _DB.Connect("tcp://127.0.0.1:3306", "root", "11JustLikeYou11", "messeger_server_db", "users");
	
	//TODO: ������� � ��
	auto Logins = _DB.FillLogins();
	//TODO: ���������� ���
	size_t LoginsSize = Logins.size();
	//���� ����������, �� ���������� ���������, ��� ��� �� ����� ��������
	//TODO: ������� hash table ��� ����� ������
	for (size_t i = 0; i < LoginsSize; ++i) {
		_Accounts[Logins[i].first] = new Account{ Logins[i].first, std::move(Logins[i].second) };
	}
}


MessengerEngine::~MessengerEngine()
{
}

//TODO: ������������ ������� � ������� ������
bool MessengerEngine::Login(Client* client, const std::string& entered_login, const std::string& entered_password)
{
	/*auto res = _Accounts.find(entered_login);
	//�� �����
	if (res == _Accounts.end()) {
		return false;
	}
	//������� ������
	auto account = res->second;

	//������ �� ����� ���� ��������� � 2 ������� ������� ������������
	if (client->_LoggedIn)
		return false;
	//���� ��� � ������ �������� ����� ���� ��������� ������ 1 ������
	if (account->_Online) {
		return false;
	}*/

	//TODO: this can only check, that pass is apply to login
	//we can take more info forom this process
	uint32_t FindId = CheckAccount(entered_login, entered_password);
	
	if ( FindId != INVALID_ID ) {
		//"������ � �������"
		BindClientToAcc(client, _Accounts[FindId]);
	}
	else
		//������������ ������
		return false;
}

void MessengerEngine::Logout(Client* client)
{
#ifdef _STATE_MESSAGE_
	std::cout << client->_Account->_Login << "- logged out\n";
#endif // _STATE_MESSAGE_
	//��������, �����-�� ����� �������� ������������������
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


void MessengerEngine::AnalyzePacket(Client* client,size_t size)
{
	if (!Deserialization::PacketCheckup(client->_ReadBuff.c_array(), size))
		//TODO: log
		return;
	//TODO: divide this
	switch (Deserialization::PaketType(client->_ReadBuff.c_array(), size))
	{
	case (int)PacketTypes::Login: 
	{
		std::string GuessLogin;
		std::string GuessPassword;
		int res = Deserialization::OnLogin(client->_ReadBuff.c_array(), size, GuessLogin, GuessPassword);
		if (res == (int)Deserialization::Result::Ok)
			//TODO: do smth with result
			Login(client, GuessLogin, GuessPassword);
	}break;

	case (int)PacketTypes::Logout:
	{
		//...
		Deserialization::OnLogout();
		Logout(client);
	}

	case (int)PacketTypes::Message:
	{
		uint32_t from, to,mess_size;
		char* Message = NULL;

		int res = Deserialization::OnMessage(client->_ReadBuff.c_array(), size, from, to, mess_size, &Message);
		//TODO: offline message
		if (res == (int)Deserialization::Result::Ok)
		{
			//TODO: rewrite this
			std::string Login;
			for (auto a : _Accounts) {
				if (a.second->ID == to)
					Login = a.second->_Login;
			}

			if (Login != "")
				break;
			else {
				if (_Accounts[Login]->_Online) {
					memcpy(_Accounts[Login]->_Client->_WriteBuf.c_array(), client->_ReadBuff.c_array(), size);
					
				}
			}
		}

	}
		
		break;

	default:
		break;
	}
}