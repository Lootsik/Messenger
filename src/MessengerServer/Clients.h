#pragma once
#include <string>
#include <boost\asio.hpp>
#include <boost\array.hpp>
#include <stdint.h>
#include "../PacketFormat/PacketFormat.h"
struct Client;


struct Account {
	Account(uint32_t id,const std::string& login)
		:ID{ id },_Login {	login  } {}
	std::string _Login;
	uint32_t ID;

	Client* _Client = nullptr;
	//TODO: field _Client describes filed below, need to delete this later
	//we just have 7 extra bytes,coz allignment
	bool _Online = false;
	//+++++++++++
};

struct Client {
	static const size_t Buffsize = MaxPacketSize;
	using Storage = typename boost::array<char, Buffsize>;

	Client(boost::asio::io_service& service) :_Socket{ service } {}

	size_t BytesWrite;
	size_t BytesRead;
	
	boost::asio::ip::tcp::socket _Socket;

	Storage _WriteBuf;
	Storage _ReadBuff;

	Account* _Account = nullptr;
	//TODO: same
	bool _LoggedIn = false;
};
