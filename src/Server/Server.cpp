#pragma once


#define _SCL_SECURE_NO_WARNINGS
#include <iostream>
#include <map>
#include <string>
#include <algorithm>
//�������� �� ������ ��������� ������
#include <vector>


#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>


#include "Server.h"

using namespace boost::asio;
using boost::system::error_code;


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


Server::Server(boost::asio::io_service& service, const unsigned short port) try
	: _Service{ service }, _Port{ port },
	_Acceptor{ service,ip::tcp::endpoint{ ip::tcp::v4(),_Port } },
	//TODO: ��������� ��� �������� � ��������
	_DB{ "tcp://127.0.0.1:3306", "root", "11JustLikeYou11" ,"messeger_server_db" ,"users" }
{
	boost::asio::ip::tcp::acceptor acceptor{ service,ip::tcp::endpoint{ ip::tcp::v4(),_Port } };
	

	//TODO: ������� � ��
	std::vector<std::string> Logins = _DB.FillLogins();
	//TODO: ���������� ���
	size_t LoginsSize = Logins.size();
	//���� ����������, �� ���������� ���������, ��� ��� �� ����� ��������
	//TODO: ������� hash table ��� ����� ������
	for (size_t i = 0; i < LoginsSize; ++i) {
		_Accounts[Logins[i]] = new Account{ std::move(Logins[i])};
	}
	
}
catch (...)
{
	std::cerr << "Fatal error\n";
}


void Server::Login(Client* client, const std::string& entered_login, const std::string& entered_password)
{
	auto res = _Accounts.find(entered_login);
	//�� �����
	if (res == _Accounts.end()) {
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

void Server::Logout(Client* client)
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
void Server::ProcessMessage(Client* client, size_t size)
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
				_MessageToAccProcess(client, size);
			//������ ��������� 
			break;

		default:
			break;
		}
	}
}

void Server::AcceptMessage(Client* client, const boost::system::error_code& err_code, size_t bytes)
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


void Server::AcceptClients(Client* client, const boost::system::error_code& err)
{
	//����� ���������� � ����� ������� 
	std::cout << "New client: " << *client << '\n';
	//���������� ����������� �������� ��� ��������� �������(�������, ����� ��������� �� ������� � ������ �������)
	client->_Socket.async_read_some(buffer(client->_Buff), boost::bind(&Server::AcceptMessage, this, client, _1, _2));
	//���������� ������� � ������ (��  ��������, �� ������ ��� ������ ��� ����������)

	_Clients.push_back(client);

	//������ ������ �������
	Client* new_Client = new Client{ _Service };

	error_code error;
	//������������ ���������� ������ ��������
	_Acceptor.async_accept(new_Client->_Socket, boost::bind(&Server::AcceptClients, this, new_Client, _1), error);
	if (error)
		throw;
}

void Server::StartAccept()
{
	Client* new_Client = new Client{ _Service };
	error_code error;
	//������������ ���������� ������ ��������
	_Acceptor.async_accept(new_Client->_Socket, boost::bind(&Server::AcceptClients, this, new_Client, _1), error);
	if (error)
		throw;
}

void Server::_WriteHandler(const error_code& err, size_t bytes)
{
	if (!err) {
		std::cout << "Sended ok\n";
	}
	else {
		std::cout << "Send error\n";
	}
}


bool Server::_PasswordCheck(const Account* account, const std::string& entered_login, const std::string& entered_password)
{
	//����������
	return _DB.IsCorrect(entered_login,entered_password);
}

//����� �������� �������
void Server::_EraseClient(Client* client, const int cause)
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
	if (client->_LoggedIn)
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

void Server::_LoginMessageProcess(Client* client, const size_t size)
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
void Server::_MessageToAccProcess(Client* client, const size_t size)
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
	auto res = _Accounts.find(reciver);
	//�� �������
	if (res == _Accounts.end())
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
			boost::bind(&Server::_WriteHandler, this, _1, _2));
	}
}
