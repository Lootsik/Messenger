#pragma once
#include <string>
#include <boost\asio.hpp>
#include <boost\array.hpp>
#include <stdint.h>
struct Client;


struct Account {
	Account(uint32_t id,const std::string& login)
		:ID{ id },_Login {	login  } {}
	uint32_t ID;
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