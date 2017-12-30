#pragma once
#define _STATE_MESSAGE_

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <map>
#include <string>
#include "Clients.h"
#include "MessengerEngine.h"

std::ostream& operator<<(std::ostream& os, const Client& client);

struct Server {
	unsigned short _Port;

	boost::asio::io_service& _Service;
	boost::asio::ip::tcp::acceptor _Acceptor;

	std::vector<Client*> _Clients{};
	MessengerEngine* _MessagerEngine;

	const char* ConfigFilename = "MessengerServer.cfg";

	Server(boost::asio::io_service& service);
	bool SetPort(const unsigned short port);
	void SetMessageEngine(MessengerEngine* me) { _MessagerEngine = me; }
	bool LoadFromConfig();


	bool Start();

	void Send(Client* client);
private:
	//for internal use only

	void CALLBACK AcceptMessage(Client* client, const boost::system::error_code& err_code, size_t bytes);
	void CALLBACK AcceptClients(Client* client, const boost::system::error_code& err);
	void CALLBACK WriteHandler(const boost::system::error_code& err, size_t bytes);

	void SolveProblemWithClient(Client* client, const boost::system::error_code& err_code);
	void DeleteClient(Client* client);	
};
