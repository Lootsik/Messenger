#pragma once
#include <boost\thread\mutex.hpp>
#include <boost\asio.hpp>
#include <boost\thread.hpp>
#include <boost\array.hpp>

#include <GlobalInfo.h>
#include <Protocol\TransferredData.h>
#include "Query.h"

using EvetQuery = typename Query<TransferredData*>;
using namespace boost::asio;

struct APIData;

class MessengerAPI
{
public:
	MessengerAPI();
	~MessengerAPI();

	int Connect(const std::string& Address, unsigned short port);
	void TryLogin(const std::string& Login, const std::string& Pass);

	bool TryGetMessage(); 
	//Connect to server
	//Authorization
	//Message


	void(*_Callback)(uint32_t, TransferredData*);
private:
	

	void _NewEvent(const boost::system::error_code& err_code, size_t bytes);
	void _WriteHandler(const boost::system::error_code& err_code, size_t bytes);
	bool _Working = false;
	io_service service;
	ip::tcp::endpoint ep;
	ip::tcp::socket sock{ service };
	boost::thread ThreadWorker;
	uint32_t ID;
	boost::array <Byte, 512> Buff;
};

