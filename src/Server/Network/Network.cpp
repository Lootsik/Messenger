#pragma once
#define _SCL_SECURE_NO_WARNINGS

#include <string>
#include <algorithm>
#include <vector>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/error.hpp>
#include <Logger/Logger.h>
#include "Network.h"


using namespace boost::asio;
using boost::system::error_code;
using namespace Logger;

//TODO: add some error checking


//*****************************
//	Network settings
//*****************************

Network::Network(boost::asio::io_service& service)
	  : _Service{ service },
		_Acceptor{ service },
		_Timer{ service } {}


//TODO: rewrite this
//NOTE: add new settings to config

bool Network::SetPort(const unsigned short port)
{
	ip::tcp::endpoint point{ ip::tcp::v4(),port };
	_Acceptor.open(point.protocol());
	error_code err;
	//trying 
	_Acceptor.bind(point, err);
	if (err)
	{
#if _LOGGING_
		Log(Error, "Cannot change port to  %d. Code: %d, Message: %s",
							port,err.value(),err.message());
#endif
		return false;
	}
	
	_Acceptor.listen();
	_Port = port;

#if _LOGGING_
	Log(Success, "Port set to %d", port);
#endif
	return true;
}

void Network::_LastSeenNow(PConnection& connection)
{
	connection->LastTime() = boost::posix_time::microsec_clock::local_time();
}

//*****************************
//	Start server
//*****************************
//actually starts accept connections
bool Network::Start()
{
	PConnection new_Connection{ new Connection{ _Service } };
	error_code error;
	//register new Network/Connection.handler function
	_Acceptor.async_accept(new_Connection->Socket(), boost::bind(&Network::_AcceptConnections, this, new_Connection, _1), error);

	_BindTimer();

#if _LOGGING_
	if (!error)
		LogBoth(Success, "Server start working on port %d", _Acceptor.local_endpoint().port());
	else
		LogBoth(Error, "Server cannot start accept: #%d:%s", error.value(), error.message().c_str());
#endif
	return !error;
}


//*****************************
//	Sending messages
//*****************************

//message already in Write buffer
void Network::Send(PConnection& connection)
{
	connection->Socket().async_write_some(
						buffer(connection->WriteBuffer(), connection->BytesToWrite()),
						boost::bind(&Network::_WriteHandler, this, _1, _2));
}

//*******************************
//	Accept message/cliet handlers
//*******************************

//TODO: change to refferense
void Network::_AcceptConnections(PConnection connection, const boost::system::error_code& err)
{
	// add async operation
	connection->Socket().async_read_some(buffer(connection->ReadBuffer()), boost::bind(&Network::_AcceptMessage, this, connection, _1, _2));

#if _LOGGING_
	Log(Action, "New connection: %s", connection->ConnectionString().c_str());
#endif
	//adding clients to list

	_LastSeenNow(connection);
	_Connections.push_back(connection);

	//new client
	PConnection new_Connection{ new Connection{ _Service } };
	error_code error;

	//registering next connect handler 
	_Acceptor.async_accept(new_Connection->Socket(), 
							boost::bind(&Network::_AcceptConnections, this, new_Connection, _1),
							error);

	//TODO: fix this
	if (error)
		throw;
}

void Network::_OnTimerCheck()
{
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	if (_Connections.size() != 0) 
	{
		size_t i = _Connections.size();
		while(i!=0)
		{
			--i;
			if ((now - _Connections[i]->LastTime()).total_milliseconds() > _Timeout)
			{
				Log(Action, "[%s] - Timeout", _Connections[i]->ConnectionString().c_str());
				_DeleteConnection(_Connections[i]);
			}
		}
	}
	_BindTimer();
}

void Network::_BindTimer()
{
	_Timer.expires_from_now(boost::posix_time::millisec(_Timeout));
	_Timer.async_wait(boost::bind(&Network::_OnTimerCheck, this));
}

//TODO: rewrite this
void  Network::_AcceptMessage(PConnection connection, const boost::system::error_code& err_code, size_t bytes)
{
	if (err_code) {
		//TODO: what we should do?
		_SolveProblemWithConnection(connection, err_code);
		return;
	}

	// mark new time
	_LastSeenNow(connection);
	//set new number of read bytes
	connection->SetBytesToRead( bytes );


#if (_LOGGING_) && (_PACKET_TRACE_)
	if(!err_code)
		Log(Action, "[%s] - message %Iu recived", connection->ConnectionString().c_str(),
												  bytes);
	else 
		Log(Mistake, "[%s] - on message error: #%d: %s", connection->ConnectionString().c_str(),
														 err_code.value(), 
														 err_code.message().c_str());
#endif

	connection->Socket().async_read_some( buffer(connection->ReadBuffer()),
									boost::bind(&Network::_AcceptMessage,this, connection, _1, _2) );

	_MessagerEngine->AnalyzePacket(connection);
}

//*****************************
//	Write handler
//*****************************

//TODO: add some
void  Network::_WriteHandler(const error_code& err, size_t bytes)
{
#if (_LOGGING_) && (_PACKET_TRACE_)
	if (err)
	{
		Log(Mistake, "Packet cannot be sended: %d: %s", err.value(), err.message().c_str());
	}
	else 
		Log(Action, "Packet is sended");
#endif
}


//*****************************
//	Managing connections
//*****************************

void Network::_SolveProblemWithConnection(PConnection& connection, const boost::system::error_code& err_code)
{
	//kick by default
	switch (err_code.value())
	{
	case error::connection_reset:

		_DeleteConnection(connection);

		break;

	case error::operation_aborted:
		break;

	default:
#if _LOGGING_
		Log(Mistake, "[%s] - Unexpected error: #%d: %s", connection->ConnectionString().c_str(), err_code.value(), err_code.message());
#endif // _STATE_MESSAGE
		_DeleteConnection(connection);

		break;
	}
}

//check 
void Network::_DeleteConnection(PConnection& connection)
{
	//TODO: change this
	if (connection->Account().Online()) {
#if _LOGGING_
		LogBoth(Action, "[%s] - Logout", connection->ConnectionString().c_str());
#endif // _LOGGING_
		_MessagerEngine->OnLogout(connection);
	}

#if _LOGGING_
	Log(Action, "[%s] - Disconnected", connection->ConnectionString().c_str());
#endif // _STATE_MESSAGE

	error_code err;
	if (connection->Socket().is_open())
	{
		connection->Socket().shutdown(ip::tcp::socket::shutdown_both, err);
		if(err)
			Log(Error, "Socket cannot be shutdowned %s", connection->ConnectionString().c_str());

		connection->Socket().close(err);
		if(err)
			Log(Error, "Socket cannot be shutdowned %s", connection->ConnectionString().c_str());

	}


	//ищем клиента в списке клиетов
	auto res = find(_Connections.begin(), _Connections.end(), connection);
	//
	if (res == _Connections.end())
	{
		Log(Error, "Client should have been deleted but he is not in list %s", connection->ConnectionString().c_str());
		return;
	}

	//delete form list
	_Connections.erase(res);

	return;
}