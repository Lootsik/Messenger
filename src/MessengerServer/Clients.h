#pragma once
#include <string>
#include <boost\asio.hpp>
#include <boost\array.hpp>
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
	static const size_t Buffsize = 256;
	using Storage = typename boost::array<char, Buffsize>;

	Client(boost::asio::io_service& service) :_Socket{ service } {}

	boost::asio::ip::tcp::socket _Socket;
	Account* _Account = nullptr;
	bool _LoggedIn = false;
	Storage _ReadBuff;
	Storage _WriteBuf;
	//стоит ввести сюда количество считаных/записаных байтов

};