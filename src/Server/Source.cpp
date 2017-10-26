//#define _WIN32_WINNT

#define _STATE_MESSAGE_
#define _SCL_SECURE_NO_WARNINGS

#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <map>
#include <string>

#include <algorithm>
//�������� �� ������ ��������� ������
#include <vector>

using namespace boost::asio;
using boost::system::error_code;
boost::asio::io_service service;
const unsigned short port = 8021;

//������ ��������� 
//2 ������� - ����� ���� ���������,  ����������� � '-'
//������, ������� ������������� ���� ���������
//����� �������� ������ �����-��

boost::asio::ip::tcp::acceptor acceptor{ service,ip::tcp::endpoint{ ip::tcp::v4(),port } };
const size_t buffsize = 128;

struct Client;

struct Account {
	Account(std::string& login, std::string& password)
		:_Login{ login }, _Password{ password } {}
	std::string _Login;
	std::string _Password;
	bool _Online = false;
	Client* _Client = nullptr;
	//+++++++++++
};
struct Client {
	Client() {}
	Account* _Account = nullptr;
	bool _LoggedIn = false;
	ip::tcp::socket _Socket = ip::tcp::socket{ service };
	boost::array<char, buffsize> _Buff;
	boost::array<char, buffsize> _WriteBuff;
	//+++++++++++
	//���� ��������� ������ 1 ������, ��� ����� ����� ������
};
std::ostream& operator<<(std::ostream& os, const Client& client)
{
	//� ����������� �� ����, ������ �� ������ � �������, ������� ���� �����, ���� �����
	if (client._LoggedIn)
		os << client._Account->_Login;
	else
		os << client._Socket.remote_endpoint().address().to_string() << ':'
		<< client._Socket.remote_endpoint().port();

	return os;
}

struct Server {
	class ClientInAcc {};
	class AccountAlreadyOnline{};
	class WrongPassword {};
	class WrongLogin {};
	
	Server() : _Clients{} {
		//����� ������� �������� � �����, ��

		Accounts["Devid_nv"] = new Account{ std::string{"Devid_nv"} ,std::string{"1234"} };
		Accounts["Sanya228"] = new Account{ std::string{ "Sanya228" } ,std::string{ "3228" } };
	}

	//��� ������� ����� ������� ������ ��� ���-�� ������
	std::vector<Client*> _Clients;
	//� ������ ��� �������, �� ����� 
	std::map<std::string, Account*> Accounts;


	void Login(Client* client, const std::string& entered_login, const std::string& entered_password)
	{
		auto res = Accounts.find(entered_login);
		//�� �����
		if (res == Accounts.end()) {
			throw WrongLogin{};
		}
		//������� ������
		auto account = res->second;
		
		//������ �� ����� ���� ��������� � 2 ������� ������� ������������
		if (client->_LoggedIn)
			throw ClientInAcc{};

		//���� ��� � ������ �������� ����� ���� ��������� ������ 1 ������
		if (account->_Online) {
			throw AccountAlreadyOnline{};
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
			throw WrongPassword{};
	}

	void Logout(Client* client)
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

	//��������� ���������
	void ProcessMessage(Client* client, size_t size)
	{
#ifdef _STATE_MESSAGE_

		std::cout << *client << " : Message\n[";
		std::cout.write(client->_Buff.data(), size);
		std::cout << "]\n";

#endif // _STATE_MESSAGE_

		auto& message = client->_Buff;
		
		//���������� ���������, ����� ������� ������������
		if (size < 2)
			return;

		//���� ������ ������ �� '-' - �������� ������ ���������
		if (message[0] == '-') {
			switch (message[1])
			{
			case 'a':
				//���������
				_LoginMessageProcess(client, size);
				break;

			case 'q':
				//������� � ��������
				Logout(client);
				break;

			case 'm':
				if (client->_LoggedIn)
					_MessageToAccProcess(client,size);
				//������ ��������� 
				break;
				
			default:
				break;
			}
		}

	}

	void AcceptMessage(Client* client, const boost::system::error_code& err_code,size_t bytes)
	{
		//������, ����� ������� �������
		if (err_code) {
			_EraseClient(client, 0);
			return;
		}
		
		//��������� ���������
		ProcessMessage(client, bytes);
		//������������ ��������� ���������
		client->_Socket.async_read_some(buffer(client->_Buff), boost::bind(&Server::AcceptMessage, this, client, _1, _2));
	}


	void AcceptClients(Client* client, const boost::system::error_code& err)
	{
		//����� ���������� � ����� ������� 
		std::cout << "New client: " << client->_Socket.remote_endpoint().address().to_string() << ':' << client->_Socket.remote_endpoint().port() << '\n';
		//���������� ����������� �������� ��� ��������� �������(�������, ����� ��������� �� ������� � ������ �������)
		client->_Socket.async_read_some(buffer(client->_Buff), boost::bind(&Server::AcceptMessage, this, client, _1, _2));
		//���������� ������� � ������ (��  ��������, �� ������ ��� ������ ��� ����������)

		_Clients.push_back(client);

		//������ ������ �������
		Client* new_Client = new Client{};
	
		error_code error;
		//������������ ���������� ������ ��������
		acceptor.async_accept(new_Client->_Socket, boost::bind(&Server::AcceptClients, this, new_Client, _1), error);
	}

	void _WriteHandler(const error_code& err, size_t bytes)
	{
		if (!err) {
			std::cout << "Sended ok\n";
		}
		else {
			std::cout << "Send error\n";
		}
	}

private:
	//������ �������� ����� ��������						�������� ��� ��
	bool _PasswordCheck(const Account* account, const std::string& entered_login,const std::string& entered_password)
	{
		//����������
		return account->_Password == entered_password;
	}

	//����� �������� �������
	void _EraseClient(Client* client, const int cause)
	{
#ifdef _STATE_MESSAGE_
		std::cout << *client << " - disconnected\n";
#endif // _STATE_MESSAGE


		error_code err;
		//������������� ��� �������� ������-������ ��� ������
		client->_Socket.shutdown(ip::tcp::socket::shutdown_both, err);
		//assert ����� ��������
		//���� �� ���� ��� ��������� ������������ ����� ��������
		assert(!err);

		//������� � ��������
		if(client->_LoggedIn)
			Logout(client);

		//���� ������� � ������ �������
		auto res = find(_Clients.begin(), _Clients.end(), client);
		//���� ����� ������� � ������ ���, ���-�� ����� �� ���
		assert(res != _Clients.end());
		
		//������� ������� �� ������
		_Clients.erase(res);

		//��������� �����
		client->_Socket.close();
		delete client;
		return;
	}

	void _LoginMessageProcess(Client* client, const size_t size)
	{
		std::string message(client->_Buff.c_array(), client->_Buff.c_array() + size);

		//� ������ -a - ���� �����������
		
		//string::npos - �� �������

		
		size_t space_after_login = message.find(" ", 4);
		//�� �����
		if (space_after_login == std::string::npos)
			return;
		std::string entered_login{ message.begin() + 3 , message.begin() + space_after_login };
		
		size_t space_after_pass = message.find(" ", space_after_login + 1);
		//�� �����
		if (space_after_pass == std::string::npos)
			return;
		std::string entered_password{ message.begin() + space_after_login + 1,message.begin() + space_after_pass };

			//�����
		try
		{
			//���������
			Login(client, entered_login, entered_password);
			std::cout << client->_Account->_Login << " - online\n";
		}
		catch (...) {
			//... ��� �� ����� ���������� � �������, ��������, ������� ���������
			//�� ���� ��������� ���������
#ifdef _STATE_MESSAGE_	
			std::cout << *client << " - Wrong login or password\n";
#endif
		}
	}

	
	//��� clietn �����������
	void _MessageToAccProcess(Client* client, const size_t size)
	{
		//��������� ������
		if (size < 3)
			return;

		std::string message(client->_Buff.c_array(), client->_Buff.c_array() + size);


		size_t space_after_reciver_login = message.find(" ", 4);
		//string::npos - �� �������
		if (space_after_reciver_login == std::string::npos)
			return;


		std::string reciver{ message.begin() + 3 , message.begin() + space_after_reciver_login };
		//���� ������� �� ������
		auto res = Accounts.find(reciver);
		//�� �������
		if (res == Accounts.end())
			return;

		auto account = res->second;
		
		//�������� ������� ���������

		//���� ��� ����� ���������� ��������� ������ ���, ��� ������
		if (account->_Online) {
			error_code err;
			std::string substring = message.substr(space_after_reciver_login);
			std::string st = client->_Account->_Login + std::string{ ":" }+substring;
			if (st.size() > buffsize + 20) {
				std::cout << "Message too long\n";
				return;
			}
			//client->_WriteBuff 
			//std::copy()
			//��������
			std::copy(st.begin(), st.end(), client->_WriteBuff.c_array());
			account->_Client->_Socket.async_write_some(
										buffer(client->_WriteBuff, st.size()),
										boost::bind(&Server::_WriteHandler,this,_1,_2 ));
		}

	}

};
//�������

//����������

int main()
{
	Server server;
	
	Client* new_client = new Client{};
	error_code err_code;
	acceptor.async_accept(new_client->_Socket, boost::bind(&Server::AcceptClients, &server, new_client , _1));
		
	service.run();


}