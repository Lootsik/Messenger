#define _CRT_SECURE_NO_WARNINGS
#include "MessengerAPI.h"
#include <stdio.h>

#include <Protocol\LoginRequest.h>
MessengerAPI::MessengerAPI()
{
}


MessengerAPI::~MessengerAPI()
{
}

void MessengerAPI::_NewEvent(const boost::system::error_code& err_code, size_t bytes)
{
	printf("Recived %d bytes\n", bytes);
	//to queue here
}
void MessengerAPI::_WriteHandler(const boost::system::error_code& err_code, size_t bytes)
{
	printf("Writed %d bytes\n", bytes);
}


int MessengerAPI::Connect(const std::string& Address, unsigned short port)
{
	ep = ip::tcp::endpoint{ ip::address::from_string(Address), port };

	boost::system::error_code err;
	sock.connect(ep, err);
	if (!err)
	{
		_Working = true;
		
		sock.async_read_some(buffer(Buff), boost::bind(&MessengerAPI::_NewEvent,this, _1, _2));
		ThreadWorker = boost::thread{ boost::bind(&boost::asio::io_service::run, &service) };
	}

	return !err;
}

void MessengerAPI::TryLogin(const std::string& Login, const std::string& Pass)
{
	LoginRequest Request{ Login, Pass };
	Request.ToBuffer(Buff.c_array());
	sock.async_write_some(boost::asio::buffer(Buff, Request.NeededSize()), 
						  boost::bind(&MessengerAPI::_WriteHandler, this, _1, _2));
}