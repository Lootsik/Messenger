#pragma once
#define _STATE_MESSAGE_ 1
#define _PACKET_TRACE_ 0

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <map>
#include <string>
#include "Clients.h"
#include "MessengerEngine.h"

std::string ClientString(const Client* client);

struct Server {
	unsigned short _Port;

	boost::asio::io_service& _Service;
	boost::asio::ip::tcp::acceptor _Acceptor;

	std::vector<Client*> _Clients{};
	MessengerEngine* _MessagerEngine;

	Server(boost::asio::io_service& service);
	bool SetPort(const unsigned short port);
	void SetMessageEngine(MessengerEngine* me) { _MessagerEngine = me; }
	bool LoadFromConfig(const char*  ConfigFilename );

	bool Start();

	void Send(Client* client);
private:
	//for internal use only
	void AcceptMessage(Client* client, const boost::system::error_code& err_code, size_t bytes);
	void AcceptClients(Client* client, const boost::system::error_code& err);
	void WriteHandler(const boost::system::error_code& err, size_t bytes);

	void SolveProblemWithClient(Client* client, const boost::system::error_code& err_code);
	void DeleteClient(Client* client);	
};
