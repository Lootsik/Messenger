#pragma once

#define _SCL_SECURE_NO_WARNINGS
#include <iostream>
#include <map>
#include <string>
#include <algorithm>
//заменить на другую структуру данных
#include <vector>


#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>


#include "Server.h"

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
	_Acceptor{ service,ip::tcp::endpoint{ ip::tcp::v4(),port } }
{
	boost::asio::ip::tcp::acceptor acceptor{ service,ip::tcp::endpoint{ ip::tcp::v4(),port } };
	//позже сделать загрузку с файла, дб
	_Accounts["Devid_nv"] = new Account{ std::string{ "Devid_nv" } ,std::string{ "1234" } };
	_Accounts["Sanya228"] = new Account{ std::string{ "Sanya228" } ,std::string{ "3228" } };
}
	//


void Server::Login(Client* client, const std::string& entered_login, const std::string& entered_password)
{
	auto res = _Accounts.find(entered_login);
	//не нашли
	if (res == _Accounts.end()) {
		throw WrongLogin{};
	}
	//аккаунт найден
	auto account = res->second;

	//клиент не может быть подключён к 2 учётным записям одновременно
	if (client->_LoggedIn)
		throw ClientInAcc{};

	//пока что к одному аккаунту может быть подключён только 1 клиент
	if (account->_Online) {
		throw AccountAlreadyOnline{};
	}

	//найдено, проверяем пароль
	if (_PasswordCheck(res->second, entered_login, entered_password)) {
		//"входим в аккаунт"
		client->_Account = account;
		account->_Client = client;

		client->_LoggedIn = true;
		account->_Online = true;

	}
	else
		//неправильный пароль
		throw WrongPassword{};
}

void Server::Logout(Client* client)
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

	//обработка сообщения
void Server::ProcessMessage(Client* client, size_t size)
{
#ifdef _STATE_MESSAGE_
	std::cout << *client << " : Message\n[";
	std::cout.write(client->_Buff.data(), size);
	std::cout << "]\n";
#endif // _STATE_MESSAGE_

	auto& message = client->_Buff;

	//достаточно маленькое, чтобы считать неправильным
	if (size < 2)
		return;

	//если первый символ не '-' - неверный формат сообщения
	if (message[0] == '-') {
		switch (message[1])
		{
		case 'a':
			//логинимся
			_LoginMessageProcess(client, size);
			break;

		case 'q':
			//выходим с аккаунта
			Logout(client);
			break;

		case 'm':
			if (client->_LoggedIn)
				_MessageToAccProcess(client, size);
			//просто сообщение 
			break;

		default:
			break;
		}
	}
}

void Server::AcceptMessage(Client* client, const boost::system::error_code& err_code, size_t bytes)
{
	//ошибка, нужно удалить клиента
	if (err_code) {
		_EraseClient(client, 0);
		return;
	}

	//обработка сообщения
	ProcessMessage(client, bytes);
	//регистрируем следующее сообщение
	client->_Socket.async_read_some(buffer(client->_Buff), boost::bind(&Server::AcceptMessage, this, client, _1, _2));
}


void Server::AcceptClients(Client* client, const boost::system::error_code& err)
{
	//вывод информации о новом клиенте 
	std::cout << "New client: " << client->_Socket.remote_endpoint().address().to_string() << ':' << client->_Socket.remote_endpoint().port() << '\n';
	//добавление асинхронной операции для принятого клиента(страшно, нужно упростить мб вывести в другую функцию)
	client->_Socket.async_read_some(buffer(client->_Buff), boost::bind(&Server::AcceptMessage, this, client, _1, _2));
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


bool Server::_PasswordCheck(const Account* account, const std::string& entered_login, const std::string& entered_password)
{
	//сравниваем
	return account->_Password == entered_password;
}

//можно добавить причину
void Server::_EraseClient(Client* client, const int cause)
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
		Logout(client);

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

void Server::_LoginMessageProcess(Client* client, const size_t size)
{
	std::string message(client->_Buff.c_array(), client->_Buff.c_array() + size);
	//в начале -a - знак авторизации
	//string::npos - не найдено


	size_t space_after_login = message.find(" ", 4);
	//не нашло
	if (space_after_login == std::string::npos)
		return;
	std::string entered_login{ message.begin() + 3 , message.begin() + space_after_login };
	size_t space_after_pass = message.find(" ", space_after_login + 1);
	//не нашло
	if (space_after_pass == std::string::npos)
		return;
	std::string entered_password{ message.begin() + space_after_login + 1,message.begin() + space_after_pass };

	//логин
	try
	{
		//логинимся
		Login(client, entered_login, entered_password);
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


	//тут clietn авторизован
void Server::_MessageToAccProcess(Client* client, const size_t size)
{
	//маленький размер
	if (size < 3)
		return;

	std::string message(client->_Buff.c_array(), client->_Buff.c_array() + size);


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
		if (st.size() > buffsize + 20) {
			std::cout << "Message too long\n";
			return;
		}
		//client->_WriteBuff 
		//std::copy()
		//копируем
		std::copy(st.begin(), st.end(), client->_WriteBuff.c_array());
		account->_Client->_Socket.async_write_some(
			buffer(client->_WriteBuff, st.size()),
			boost::bind(&Server::_WriteHandler, this, _1, _2));
	}
}
