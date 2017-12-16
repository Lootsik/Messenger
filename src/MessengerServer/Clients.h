#pragma once
#include <string>
#include <boost\asio.hpp>
#include "Message.h"
struct Client;

struct Account {
	Account(const std::string& login)
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
	Message _Buff;
	Message _WriteBuff;
	//стоит ввести сюда количество считаных/записаных байтов
	//+++++++++++
	//если добавлять больше 1 потока, тут нужен будет мутекс
};