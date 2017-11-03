#pragma once
#define _STATE_MESSAGE_

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <map>
#include <string>
#include "Database.h"
//�������� �� ������ ��������� ������



//������ ��������� 
//2 ������� - ����� ���� ���������,  ����������� � '-'
//������, ������� ������������� ���� ���������
//����� �������� ������ �����-��

const size_t buffsize = 128;

struct Client;

struct Account {
	Account(std::string& login)
		:_Login{ login } {}
	std::string _Login;
	bool _Online = false;
	Client* _Client = nullptr;
	//+++++++++++
};
struct Client {
	Client(boost::asio::io_service& service) :_Socket{ service } {}
	boost::asio::ip::tcp::socket _Socket;

	Account* _Account = nullptr;
	bool _LoggedIn = false;
	boost::array<char, buffsize> _Buff;
	boost::array<char, buffsize> _WriteBuff;
	//+++++++++++
	//���� ��������� ������ 1 ������, ��� ����� ����� ������
};
std::ostream& operator<<(std::ostream& os, const Client& client);


struct Server {
	class ClientInAcc {};
	class AccountAlreadyOnline {};
	class WrongPassword {};
	class WrongLogin {};
	

	boost::asio::io_service& _Service;

	//��� ������� ����� ������� ������ ��� ���-�� ������
	std::vector<Client*> _Clients{};
	//� ������ ��� �������, �� ����� 
	unsigned short _Port;
	//TODO: ������� ��������� �����, ������ 
	std::map < std::string, Account*> _Accounts;
	boost::asio::ip::tcp::acceptor _Acceptor;
	Database _DB;

	Server(boost::asio::io_service& service, const unsigned short port);

	void Login(Client* client, const std::string& entered_login, const std::string& entered_password);
	
	void Logout(Client* client);
	
	//��������� ���������

	void AcceptMessage(Client* client, const boost::system::error_code& err_code, size_t bytes);

	void AcceptClients(Client* client, const boost::system::error_code& err);

	void StartAccept();


private:

	void ProcessMessage(Client* client, size_t size);
	void _WriteHandler(const boost::system::error_code& err, size_t bytes);


	//������ �������� ����� ��������						�������� ��� ��
	bool _PasswordCheck(const Account* account, const std::string& entered_login, const std::string& entered_password);

	//����� �������� �������
	void _EraseClient(Client* client, const int cause);

	void _LoginMessageProcess(Client* client, const size_t size);

	//��� clietn �����������
	void _MessageToAccProcess(Client* client, const size_t size);

};