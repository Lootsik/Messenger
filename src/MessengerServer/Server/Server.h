#pragma once
#define _LOGGING_ 1
#define _PACKET_TRACE_ 0

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <map>
#include <string>
#include <Server/Connection.h>
#include <Engine/Engine.h>

std::string ConnectionString(const Connection* Connection);

struct Server {
	unsigned short _Port;

	boost::asio::io_service& _Service;
	boost::asio::ip::tcp::acceptor _Acceptor;

	std::vector<Connection*> _Connections{};
	MessengerEngine* _MessagerEngine;

	Server(boost::asio::io_service& service);
	bool SetPort(const unsigned short port);
	void SetMessageEngine(MessengerEngine* me) { _MessagerEngine = me; }
	bool LoadFromConfig(const char*  ConfigFilename );

	bool Start();

	template<typename T>
	void _Send(Connection* connection,T Item)
	{
		
	}
	void _Send(Connection* connection);
private:
	//for internal use only
	void AcceptMessage(Connection* connection, const boost::system::error_code& err_code, size_t bytes);
	void AcceptConnections(Connection* connection, const boost::system::error_code& err);
	void WriteHandler(const boost::system::error_code& err, size_t bytes);

	void SolveProblemWithConnection(Connection* connection, const boost::system::error_code& err_code);
	void DeleteConnection(Connection* connection);
};
