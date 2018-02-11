#pragma once
#include <boost\thread\mutex.hpp>
#include <boost\asio.hpp>
#include <boost\thread.hpp>

#include <GlobalInfo.h>

using namespace boost::asio;

class MessengerAPI
{
public:
	MessengerAPI();

	int Connect(const std::string& Address, unsigned short port);


	//Connect to server
	//Authorization
	//Message

	~MessengerAPI();

private:
	bool _Working = false;
	io_service service;
	ip::tcp::endpoint ep;
	ip::tcp::socket sock{ service };

	uint32_t ID;
};

