#pragma once
#define _STATE_MESSAGE_

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <map>
#include <string>
#include "Message.h"
#include "Clients.h"
#include "MessengerEngine.h"
//заменить на другую структуру данных

std::ostream& operator<<(std::ostream& os, const Client& client);


struct Server {
	unsigned short _Port;

	boost::asio::io_service& _Service;
	boost::asio::ip::tcp::acceptor _Acceptor;

	std::vector<Client*> _Clients{};

	MessengerEngine* _MessageEngine;

	Server(boost::asio::io_service& service);
	bool SetPort(const unsigned short port);
	
	void SetMessageEngine(MessengerEngine* me) { _MessageEngine = me; }
	//обработка сообщения
	//void SendMessageTo();


	void StartAccept();

	void AcceptMessage(Client* client, const boost::system::error_code& err_code, size_t bytes);

	void AcceptClients(Client* client, const boost::system::error_code& err);

private:

	void _OnLoginMessage(Client* client, size_t size);
	void _OnLogoutMessage(Client* client, size_t size);
	//void _OnDirectMessage(Client* client, size_t size);
	
	void _ProcessMessage(Client* client, size_t size);


	void _WriteHandler(const boost::system::error_code& err, size_t bytes);

	void _SolveProblemWithClient(Client* client, const boost::system::error_code& err_code);

	void _DeleteClient(Client* client);	
};