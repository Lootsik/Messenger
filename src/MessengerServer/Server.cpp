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

std::string ClientString(const Client* client)
{
	if (client->_LoggedIn)
		return std::to_string(client->_Account->ID) + ":" + client->_Account->_Login;
	else
		return client->_Socket.remote_endpoint().address().to_string() +
				":" + std::to_string(client->_Socket.remote_endpoint().port());
}


//*****************************
//	Server settings
//*****************************

Server::Server(boost::asio::io_service& service)
	: _Service{ service },
	_Acceptor{ service }
{

}

//TODO: rewrite this
//NOTE: add new settings to config
bool Server::LoadFromConfig(const char* ConfigFilename)
{
	std::ifstream ConfigFile{ ConfigFilename };
	if (!ConfigFile.is_open())
	{
#if _STATE_MESSAGE_
		LogBoth(Error, "Configs cannot be loaded");
#endif 
		return false;
	}
	
	unsigned short port = 0;
	if (!(ConfigFile >> port))
	{
#if _STATE_MESSAGE_
		LogBoth(Error, "Configs cannot be loaded");
#endif 
		return false;

	}

	bool res = SetPort(port);
#if _STATE_MESSAGE_
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
	
#if _STATE_MESSAGE_
	if (!error)
		LogBoth(Success, "Server start working on port %d", _Acceptor.local_endpoint().port());
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


void  Server::AcceptClients(Client* client, const boost::system::error_code& err)
{
	//добавление асинхронной операции для принятого клиента(страшно, нужно упростить мб вывести в другую функцию)
	client->_Socket.async_read_some(buffer(client->_ReadBuff), boost::bind(&Server::AcceptMessage, this, client, _1, _2));
	//добавление клиента в список (не  критично, тк вызовы для сокета уже забинджены)

#if _STATE_MESSAGE_
	Log(Action, "New connection: %s:%d", client->_Socket.local_endpoint().address().to_string().c_str(),
										 client->_Socket.local_endpoint().port());
#endif

	_Clients.push_back(client);

	//создаём нового клиента
	Client* new_Client = new Client{ _Service };

	error_code error;
	//регистрируем обработчик нового коннекта
	_Acceptor.async_accept(new_Client->_Socket, boost::bind(&Server::AcceptClients, this, new_Client, _1), error);
	//TODO: fix this
	if (error)
		throw;
}

//TODO: rewrite this
void  Server::AcceptMessage(Client* client, const boost::system::error_code& err_code, size_t bytes)
{
	
	//ошибка, нужно удалить клиента
	if (err_code) {
		//TODO: стоит определить план дейстивий для конкретного случая: принимать дальше сообщения или нет
		SolveProblemWithClient(client, err_code);
		return;
	}

	client->BytesRead = bytes;

#if (_STATE_MESSAGE_) && (_PACKET_TRACE_)
	if(!err_code)
		Log(Action, "[%s] - message %Iu recived", ClientString(client).c_str(),
												  bytes);
	else 
		Log(Mistake, "[%s] - on message error: #%d: %s", ClientString(client).c_str(),
														 err_code.value(), 
														 err_code.message().c_str());
#endif
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
void  Server::WriteHandler(const error_code& err, size_t bytes)
{
#if (_STATE_MESSAGE_) && (_PACKET_TRACE_)
	if (err)
	{
		Log(Mistake, "Packet cannot be sended: %d: %s", err.value(), err.message().c_str());
	}
	else 
		Log(Action, "Packet is sended");
#endif
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

//check 
void Server::DeleteClient(Client* client)
{
#ifdef _STATE_MESSAGE_
	//TODO: its not kicked by server actually, change this
	Log(Action, "[%s] - kicked by server", ClientString(client).c_str());
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