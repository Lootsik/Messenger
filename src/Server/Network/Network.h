#pragma once
#define _LOGGING_ 1
#define _PACKET_TRACE_ 0

#include <map>
#include <string>

#include <boost/asio.hpp>
#include <boost/array.hpp>

#include <Network/Connection.h>
#include <Engine/Engine.h>

class Network {
public:
	Network(boost::asio::io_service& service);
	
	bool Start();
	
	bool SetPort(const unsigned short port);
	void SetMessageEngine(MessengerEngine* me) { _MessagerEngine = me; }
	//for engine
	void Send(PConnection& connection);
	void SetTimeout(unsigned int Timeout) { _Timeout = Timeout; }
	unsigned short GetTimeout() const { return _Timeout; }
	
	// TODO: avoid this
	PConnection* FindConnected(ID_t id)
	{
		auto find_res =  std::find_if(_Connections.begin(), _Connections.end(),
						[&](const PConnection& con)
						{
							return con->Account().ID() == id;
						});
		if (find_res == _Connections.end())
			return nullptr ;

		return find_res._Ptr ;
	}

	//milliseconds
private:
	boost::asio::io_service& _Service;
	
	boost::asio::deadline_timer _Timer;
	boost::posix_time::ptime last_ping;
	boost::asio::ip::tcp::acceptor _Acceptor;
	unsigned short _Port;
	// connection list
	std::vector<PConnection> _Connections{};
	MessengerEngine* _MessagerEngine;
	//TODO: to config
	unsigned int _Timeout = 5000000;

	//for internal use only
	void _LastSeenNow(PConnection& connection);

	void _AcceptMessage(PConnection connection, const boost::system::error_code& err_code, size_t bytes);
	void _AcceptMessageRemainder(PConnection connection, const boost::system::error_code& err_code, size_t bytes);

	void _AcceptConnections(PConnection connection, const boost::system::error_code& err);
	void _WriteHandler(const boost::system::error_code& err, size_t bytes);
	void _SolveProblemWithConnection(PConnection& conn , const boost::system::error_code& err_code);
	void _DeleteConnection(PConnection& connection);


	void _BindTimer();
	void _BindMessage(PConnection& connection);
	void _BindMessageRemainder(PConnection& connection, size_t ReceiveBytes);


	
	//erase check unautorized connectios, that dont doing nothing more that timeout
	//not for single connection, working with list
	void _OnTimerCheck();

};
