#include <vector>
#include "MessengerEngine.h"
#include "Server.h"
#include "Deserialization.h"

MessengerEngine::MessengerEngine(Server* server)
		//��������� �������� � ��������
		:_DB{ "tcp://127.0.0.1:3306", "root", "11JustLikeYou11" ,"messeger_server_db" ,"users" },_Server{server}
{
	//TODO: ������� � ��
	std::vector<std::string> Logins = _DB.FillLogins();
	//TODO: ���������� ���
	size_t LoginsSize = Logins.size();
	//���� ����������, �� ���������� ���������, ��� ��� �� ����� ��������
	//TODO: ������� hash table ��� ����� ������
	for (size_t i = 0; i < LoginsSize; ++i) {
		_Accounts[Logins[i]] = new Account{ std::move(Logins[i]) };
	}

}


MessengerEngine::~MessengerEngine()
{
}

//TODO: ������������ ������� � ������� ������
bool MessengerEngine::Login(Client* client, const std::string& entered_login, const std::string& entered_password)
{
	auto res = _Accounts.find(entered_login);
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
	}

	//�������, ��������� ������
	if (_PasswordCheck(res->second, entered_login, entered_password)) {
		//"������ � �������"
		client->_Account = account;
		account->_Client = client;

		client->_LoggedIn = true;
		account->_Online = true;

	}
	else
		//������������ ������
		return true;
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




bool MessengerEngine::_PasswordCheck(const Account* account, const std::string& entered_login, const std::string& entered_password)
{
	//compare
	return _DB.IsCorrect(entered_login, entered_password);
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

	}break;

	case (int)PacketTypes::Logout:
	{
		//...
		Deserialization::OnLogout();
		Logout(client);
	}
		
		break;

	default:
		break;
	}
}