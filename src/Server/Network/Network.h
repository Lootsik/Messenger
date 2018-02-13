#pragma once
#define _LOGGING_ 1
#define _PACKET_TRACE_ 0

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <map>
#include <string>
#include <Network/Connection.h>
#include <Engine/Engine.h>

std::string ConnectionString(const Connection* Connection);

class Network {
public:
	Network(boost::asio::io_service& service);
	bool SetPort(const unsigned short port);
	void SetMessageEngine(MessengerEngine* me) { _MessagerEngine = me; }
	bool LoadFromConfig(const char*  ConfigFilename );

	bool Start();

	void Send(Connection* connection);
private:
	unsigned short _Port;
	boost::asio::io_service& _Service;
	boost::asio::ip::tcp::acceptor _Acceptor;

	std::vector<Connection*> _Connections{};
	MessengerEngine* _MessagerEngine;

	//for internal use only
	void _AcceptMessage(Connection* connection, const boost::system::error_code& err_code, size_t bytes);
	void _AcceptConnections(Connection* connection, const boost::system::error_code& err);
	void _WriteHandler(const boost::system::error_code& err, size_t bytes);

	void _SolveProblemWithConnection(Connection* connection, const boost::system::error_code& err_code);
	void _DeleteConnection(Connection* connection);

};
