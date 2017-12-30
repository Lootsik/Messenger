#pragma once
#define _SCL_SECURE_NO_WARNINGS
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>


#include "Server.h"
#include "MessengerEngine.h"
#include "Logger.h"
using namespace boost::asio;
using boost::system::error_code;
using namespace Logger;
//TODO: add some error checking
const char* LogFilename = "Server.log";

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
//*****************************
//	Server settings
//*****************************

Server::Server(boost::asio::io_service& service)
	: _Service{ service },
	_Acceptor{ service }
{
#ifdef _STATE_MESSAGE_
	OpenLogFile(LogFilename);
#endif
}

//TODO: rewrite this
//NOTE: add new settings to config
bool Server::LoadFromConfig()
{
	std::ifstream ConfigFile{ ConfigFilename };
	if (!ConfigFile.is_open())
	{
#ifdef _STATE_MESSAGE_
		LogBoth(Error, "Configs cannot be loaded");
#endif 
		return false;
	}
	
	unsigned short port = 0;
	if (!(ConfigFile >> port))
	{
#ifdef _STATE_MESSAGE_
		LogBoth(Error, "Configs cannot be loaded");
#endif 
		return false;

	}

	bool res = SetPort(port);
#ifdef _STATE_MESSAGE_
	if (res)
		LogBoth(Success, "Configs successfully loaded.");
	else
		LogBoth(Error, "Configs cannot be loaded");
#endif 
	return res;
}

bool Server::SetPort(const unsigned short port)
{
	_Port = port;

	ip::tcp::endpoint point{ ip::tcp::v4(),port };
	_Acceptor.open(point.protocol());
	error_code err;

	_Acceptor.bind(point, err);
	_Acceptor.listen();
	/*
#ifdef _STATE_MESSAGE_
	Log(Success, "Port set to %d", port);
#endif*/
	return !err;
}

//*****************************
//	Start server
//*****************************

//actually starts accept connections
bool Server::Start()
{
	Client* new_Client = new Client{ _Service };
	error_code error;
	//register new connection handler function
	_Acceptor.async_accept(new_Client->_Socket, boost::bind(&Server::AcceptClients, this, new_Client, _1), error);
	
#ifdef _STATE_MESSAGE_
	if (!error)
		LogBoth(Success, "Server start working on %s:%5d", _Acceptor.local_endpoint().address().to_string().c_str(),
			_Acceptor.local_endpoint().port());
	else
		LogBoth(Error, "Server cannot start accept: #%d:%s", error.value(), error.message().c_str());
#endif
	return !error;
}


//*****************************
//	Sending messages
//*****************************


//message already in Write buffer
void Server::Send(Client* client)
{
	client->_Socket.async_write_some(
			buffer(client->_WriteBuf, client->BytesWrite),
			boost::bind(&Server::WriteHandler, this, _1, _2));
}

//*******************************
//	Accept message/cliet handlers
//*******************************


void CALLBACK Server::AcceptClients(Client* client, const boost::system::error_code& err)
{
	//����� ���������� � ����� ������� 
	std::cout << "New client: " << *client << '\n';
	//���������� ����������� �������� ��� ��������� �������(�������, ����� ��������� �� ������� � ������ �������)
	client->_Socket.async_read_some(buffer(client->_ReadBuff), boost::bind(&Server::AcceptMessage, this, client, _1, _2));
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

void CALLBACK Server::AcceptMessage(Client* client, const boost::system::error_code& err_code, size_t bytes)
{
	//������, ����� ������� �������
	if (err_code) {
		//TODO: ����� ���������� ���� ��������� ��� ����������� ������: ��������� ������ ��������� ��� ���
		SolveProblemWithClient(client, err_code);
		return;
	}

	client->BytesRead = bytes;

	//����-��� ������ ������ �� ����� 
	std::cout << "Message from " << *client << ' ' << bytes << " bytes\n";

	//��������� ���������
	_MessagerEngine->AnalyzePacket(client, bytes);
	//��������� ���������
	//TODO: We need this?
	client->_Socket.async_read_some( buffer(client->_ReadBuff), 
									boost::bind(&Server::AcceptMessage,this, client, _1, _2) );
}

//*****************************
//	Write handler
//*****************************

//TODO: add some
void CALLBACK Server::WriteHandler(const error_code& err, size_t bytes)
{
	if (!err) {
		std::cout << "Sended ok\n";
	}
	else {
		std::cout << "Send error\n";
	}
}


//*****************************
//	Managing connections
//*****************************

void Server::SolveProblemWithClient(Client* client, const boost::system::error_code& err_code)
{
	//TODO: add some action according to error 
	//kick by default
	DeleteClient(client);
}

void Server::DeleteClient(Client* client)
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
		_MessagerEngine->Logout(client);

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
