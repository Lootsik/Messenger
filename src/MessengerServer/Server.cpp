#pragma once

//TODO: можно сделать отдельный поток, где будет обработка сообщений
#define _SCL_SECURE_NO_WARNINGS
#include <iostream>
#include <map>
#include <string>
#include <algorithm>
#include <vector>


#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>


#include "Server.h"
#include "MessengerEngine.h"
using namespace boost::asio;
using boost::system::error_code;


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


Server::Server(boost::asio::io_service& service)
	: _Service{ service },
	_Acceptor{ service}
{

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



void Server::_ProcessMessage(Client* client,size_t size)
{
	//using namespace MessageProcessor
	switch (client->_Buff.Type()) {

	case Message::MessageType::Login:
	//	_OnLogin(client);
		break;

	case Message::MessageType::Logout:
		//_OnLogout(client);
		break;

	case Message::MessageType::message_:
		//_OnMessage(client);
		break;
	default:
		//TODO: action to wrong message
		return;
	}
}





void Server::AcceptMessage(Client* client, const boost::system::error_code& err_code, size_t bytes)
{
	//ошибка, нужно удалить клиента
	if (err_code) {
		//TODO: стоит определить план дейстивий для конкретного случая: принимать дальше сообщения или нет
		_SolveProblemWithClient(client, err_code);
		return;
	}

	//пока-что просто выведём на экран 
	std::cout << "Message: " << client->_Buff.Buffer().c_array() << '\n';


	//обработка сообщения
	_ProcessMessage(client, bytes);
	//регистрируем следующее сообщение
	client->_Socket.async_read_some(buffer(client->_Buff.Buffer()), boost::bind(&Server::AcceptMessage, this, client, _1, _2));
}


void Server::AcceptClients(Client* client, const boost::system::error_code& err)
{
	//вывод информации о новом клиенте 
	std::cout << "New client: " << *client << '\n';
	//добавление асинхронной операции для принятого клиента(страшно, нужно упростить мб вывести в другую функцию)
	client->_Socket.async_read_some(buffer(client->_Buff.Buffer()), boost::bind(&Server::AcceptMessage, this, client, _1, _2));
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

void Server::StartAccept()
{
	Client* new_Client = new Client{ _Service };
	error_code error;
	//регистрируем обработчик нового коннекта
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




void Server::_SolveProblemWithClient(Client* client, const boost::system::error_code& err_code)
{
	//kick by default,TODO: add some action according to error 
	_DeleteClient(client);
}

void Server::_DeleteClient(Client* client)
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
		_MessageEngine->Logout(client);

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

void Server::_OnLoginMessage(Client* client, const size_t size)
{
	std::string entered_login;
	std::string entered_password;
	//логин
	try
	{
		//логинимся
		_MessageEngine->Login(client, entered_login, entered_password);
		std::cout << client->_Account->_Login << " - online\n";
	}
	catch (...) {
		//... это всё можно обработать и послать, например, клиенту подсказки
		//но пока дефолтное сообщение
#ifdef _STATE_MESSAGE_	
		std::cout << *client << " - Wrong login or password\n";
#endif
	}
}

void Server::_OnLogoutMessage(Client* client, size_t size)
{
	_MessageEngine->Logout(client);
}

/*
	//тут clietn авторизован
void Server::_MessageToAccProcess(Client* client, const size_t size)
{
	//маленький размер
	if (size < 3)
		return;

	std::string message(client->_Buff.Buffer().c_array(), client->_Buff.Buffer().c_array() + size);


	size_t space_after_reciver_login = message.find(" ", 4);
	//string::npos - не найдено
	if (space_after_reciver_login == std::string::npos)
		return;


	std::string reciver{ message.begin() + 3 , message.begin() + space_after_reciver_login };
	//ищем аккаунт по логину
	auto res = _Accounts.find(reciver);
	//не найдено
	if (res == _Accounts.end())
		return;

	auto account = res->second;

	//добавить очередь сообщений

	//пока что можно отправлять сообщения только тем, кто онлайн
	if (account->_Online) {
		error_code err;
		std::string substring = message.substr(space_after_reciver_login);
		std::string st = client->_Account->_Login + std::string{ ":" }+substring;
		if (st.size() > Message::Buffsize + 20) {
			std::cout << "Message too long\n";
			return;
		}
		//client->_WriteBuff 
		//std::copy()
		//копируем
		std::copy(st.begin(), st.end(), client->_WriteBuff.Buffer().c_array());
		account->_Client->_Socket.async_write_some(
			buffer(client->_WriteBuff.Buffer(), st.size()),
			boost::bind(&Server::_WriteHandler, this, _1, _2));
	}
}
*/