#pragma once
#define _LOGGING_ 1
#define _PACKET_TRACE_ 0

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <map>
#include <string>
#include <Network/Connection.h>
#include <Engine/Engine.h>

//TODO: replace calls to mem func with shared_ptr

std::string ConnectionString(const Connection* Connection);

class Network {
public:
	Network(boost::asio::io_service& service);
	bool SetPort(const unsigned short port);
	void SetMessageEngine(MessengerEngine* me) { _MessagerEngine = me; }
	bool LoadFromConfig(const char*  ConfigFilename );

	bool Start();

	void Send(PConnection& connection);
private:

	//milliseconds
	static const unsigned int Timeout = 5000;

	//timer
	boost::asio::deadline_timer _Timer;

	boost::posix_time::ptime last_ping;
	unsigned short _Port;
	boost::asio::io_service& _Service;
	boost::asio::ip::tcp::acceptor _Acceptor;

	std::vector<PConnection> _Connections{};
	MessengerEngine* _MessagerEngine;

	void _BindTimer();

	void _LastSeenNow(PConnection& connection);
	//for internal use only
	void _OnTimer();
	void _AcceptMessage(PConnection connection, const boost::system::error_code& err_code, size_t bytes);
	void _AcceptConnections(PConnection connection, const boost::system::error_code& err);

	//void _AcceptConnections(Connection* connection, const boost::system::error_code& err);
	void _WriteHandler(const boost::system::error_code& err, size_t bytes);

	void _SolveProblemWithConnection(PConnection& conn , const boost::system::error_code& err_code);
	void _DeleteConnection(PConnection& connection);

};
