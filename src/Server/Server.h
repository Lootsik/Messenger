#pragma once
#define _STATE_MESSAGE_

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <map>
#include <string>
#include "Database.h"
//заменить на другую структуру данных



//формат сообщения 
//2 символа - метка типа сообщения,  начинаеться с '-'
//данные, которые соответствуют типу сообщения
//нужно изменить формат когда-то

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
	//если добавлять больше 1 потока, тут нужен будет мутекс
};
std::ostream& operator<<(std::ostream& os, const Client& client);


struct Server {
	class ClientInAcc {};
	class AccountAlreadyOnline {};
	class WrongPassword {};
	class WrongLogin {};
	

	boost::asio::io_service& _Service;

	//для клиетов лучше сделать список или что-то другое
	std::vector<Client*> _Clients{};
	//в идеале хеш таблица, но потом 
	unsigned short _Port;
	//TODO: сделать отдельный класс, скрыть 
	std::map < std::string, Account*> _Accounts;
	boost::asio::ip::tcp::acceptor _Acceptor;
	Database _DB;

	Server(boost::asio::io_service& service, const unsigned short port);

	void Login(Client* client, const std::string& entered_login, const std::string& entered_password);
	
	void Logout(Client* client);
	
	//обработка сообщения

	void AcceptMessage(Client* client, const boost::system::error_code& err_code, size_t bytes);

	void AcceptClients(Client* client, const boost::system::error_code& err);

	void StartAccept();


private:

	void ProcessMessage(Client* client, size_t size);
	void _WriteHandler(const boost::system::error_code& err, size_t bytes);


	//способ проверки можно изменить						оставляю для дб
	bool _PasswordCheck(const Account* account, const std::string& entered_login, const std::string& entered_password);

	//можно добавить причину
	void _EraseClient(Client* client, const int cause);

	void _LoginMessageProcess(Client* client, const size_t size);

	//тут clietn авторизован
	void _MessageToAccProcess(Client* client, const size_t size);

};