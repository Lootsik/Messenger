#pragma once
#define _SCL_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>


#include "Server.h"
#include "MessengerEngine.h"
using namespace boost::asio;
using boost::system::error_code;

//TODO: add some error checking


std::ostream& operator<<(std::ostream& os, const Client& client)
{
	//в зависимости от того, ввошёл ли клиент в аккаунт, выводим либо логин, либо адрес
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
}

//NOTE: add new settings to config
bool Server::LoadFromConfig()
{
	std::ifstream ConfigFile{ ConfigFilename };
	if (!ConfigFile.is_open())
		return false;
	
	unsigned short port;
	if (!(ConfigFile >> port))
		return false;

	return SetPort(port);
}

bool Server::SetPort(const unsigned short port)
{
	_Port = port;

	ip::tcp::endpoint point{ ip::tcp::v4(),port };
	_Acceptor.open(point.protocol());
	error_code err;

	_Acceptor.bind(point, err);
	_Acceptor.listen();

	/*TODO: log*/
	return !err;
}

//*****************************
//	Start server
//*****************************

//actually starts accept connections
void Server::Start()
{
	Client* new_Client = new Client{ _Service };
	error_code error;
	//register new connection handler function
	_Acceptor.async_accept(new_Client->_Socket, boost::bind(&Server::AcceptClients, this, new_Client, _1), error);
	if (error)
		throw;
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
	//вывод информации о новом клиенте 
	std::cout << "New client: " << *client << '\n';
	//добавление асинхронной операции для принятого клиента(страшно, нужно упростить мб вывести в другую функцию)
	client->_Socket.async_read_some(buffer(client->_ReadBuff), boost::bind(&Server::AcceptMessage, this, client, _1, _2));
	//добавление клиента в список (не  критично, тк вызовы для сокета уже забинджены)

	_Clients.push_back(client);

	//создаём нового клиента
	Client* new_Client = new Client{ _Service };

	error_code error;
	//регистрируем обработчик нового коннекта
	_Acceptor.async_accept(new_Client->_Socket, boost::bind(&Server::AcceptClients, this, new_Client, _1), error);
	if (error)
		throw;
}

void CALLBACK Server::AcceptMessage(Client* client, const boost::system::error_code& err_code, size_t bytes)
{
	//ошибка, нужно удалить клиента
	if (err_code) {
		//TODO: стоит определить план дейстивий для конкретного случая: принимать дальше сообщения или нет
		SolveProblemWithClient(client, err_code);
		return;
	}

	client->BytesRead = bytes;

	//пока-что просто выведём на экран 
	std::cout << "Message from " << *client << ' ' << bytes << " bytes\n";

	//обработка сообщения
	_MessagerEngine->AnalyzePacket(client, bytes);
	//следующее сообщение
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
	//останавливаем все операции чтения-записи для сокета
	client->_Socket.shutdown(ip::tcp::socket::shutdown_both, err);
	//assert нужно заменить
	//пока не знаю как правильно обрабатывать такие ситуации
	assert(!err);

	//выходим с аккаунта
	if (client->_LoggedIn)
		_MessagerEngine->Logout(client);

	//ищем клиента в списке клиетов
	auto res = find(_Clients.begin(), _Clients.end(), client);
	//если этого клиента в списке нет, что-то пошло не так
	assert(res != _Clients.end());

	//удаляем клиента со списка
	_Clients.erase(res);

	//закрываем сокет
	client->_Socket.close();
	delete client;
	return;
}
