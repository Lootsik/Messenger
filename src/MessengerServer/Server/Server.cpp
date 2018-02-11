#pragma once
#define _SCL_SECURE_NO_WARNINGS

#include <string>
#include <algorithm>
#include <vector>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>

#include <Logger/Logger.h>
#include "Server.h"


using namespace boost::asio;
using boost::system::error_code;
using namespace Logger;
//TODO: add some error checking

//todo add ID
std::string ConnectionString(const Connection* connection)
{
	return connection->_Socket.remote_endpoint().address().to_string() +
				":" + std::to_string(connection->_Socket.remote_endpoint().port());
}


//*****************************
//	Server settings
//*****************************

Server::Server(boost::asio::io_service& service)
	: _Service{ service },
	_Acceptor{ service }{}


//TODO: rewrite this
//NOTE: add new settings to config
bool Server::LoadFromConfig(const char* ConfigFilename)
{
	std::ifstream ConfigFile{ ConfigFilename };
	if (!ConfigFile.is_open())
	{
#if _LOGGING_
		LogBoth(Error, "Configs cannot be loaded");
#endif 
		return false;
	}
	
	unsigned short port = 0;
	if (!(ConfigFile >> port))
	{
#if _LOGGING_
		LogBoth(Error, "Configs cannot be loaded");
#endif 
		return false;
	}

	bool res = SetPort(port);
#if _LOGGING_
	if (res)
		LogBoth(Success, "Configs successfully loaded.");
	else
		LogBoth(Error, "Configs cannot be loaded");
#endif 
	return res;
}

bool Server::SetPort(const unsigned short port)
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

//*****************************
//	Start server
//*****************************

//actually starts accept connections
bool Server::Start()
{
	Connection* new_Connection = new Connection{ _Service };
	error_code error;
	//register new Server/Connection.handler function
	_Acceptor.async_accept(new_Connection->_Socket, boost::bind(&Server::_AcceptConnections, this, new_Connection, _1), error);
	
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
void Server::Send(Connection* connection)
{
	connection->_Socket.async_write_some(
				buffer(connection->_WriteBuf, connection->BytesWrite),
				boost::bind(&Server::_WriteHandler, this, _1, _2));
}

//*******************************
//	Accept message/cliet handlers
//*******************************


void Server::_AcceptConnections(Connection* connection, const boost::system::error_code& err)
{
	//���������� ����������� �������� ��� ��������� �������(�������, ����� ��������� �� ������� � ������ �������)
	connection->_Socket.async_read_some(buffer(connection->_ReadBuff), boost::bind(&Server::_AcceptMessage, this, connection, _1, _2));
	//���������� ������� � ������ (��  ��������, �� ������ ��� ������ ��� ����������)

#if _LOGGING_
	Log(Action, "New connection: %s:%d", connection->_Socket.remote_endpoint().address().to_string().c_str(),
										 connection->_Socket.remote_endpoint().port());
#endif

	_Connections.push_back(connection);

	//������ ������ �������
	Connection* new_Connection = new Connection{ _Service };

	error_code error;
	//������������ ���������� ������ ��������

	_Acceptor.async_accept(new_Connection->_Socket, boost::bind(&Server::_AcceptConnections, this, new_Connection, _1), error);

	//TODO: fix this
	if (error)
		throw;
}

//TODO: rewrite this
void  Server::_AcceptMessage(Connection* connection, const boost::system::error_code& err_code, size_t bytes)
{
	//������, ����� ������� �������
	if (err_code) {
		//TODO: ����� ���������� ���� ��������� ��� ����������� ������: ��������� ������ ��������� ��� ���
		_SolveProblemWithConnection(connection, err_code);
		return;
	}

	connection->BytesRead = bytes;

#if (_LOGGING_) && (_PACKET_TRACE_)
	if(!err_code)
		Log(Action, "[%s] - message %Iu recived", ConnectionString(Connection).c_str(),
												  bytes);
	else 
		Log(Mistake, "[%s] - on message error: #%d: %s", ConnectionString(Connection).c_str(),
														 err_code.value(), 
														 err_code.message().c_str());
#endif
	//TODO: We need this?
	connection->_Socket.async_read_some( buffer(connection->_ReadBuff),
									boost::bind(&Server::_AcceptMessage,this, connection, _1, _2) );

	connection->BytesRead = bytes;
	_MessagerEngine->AnalyzePacket(connection);
}

//*****************************
//	Write handler
//*****************************

//TODO: add some
void  Server::_WriteHandler(const error_code& err, size_t bytes)
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

void Server::_SolveProblemWithConnection(Connection* connection, const boost::system::error_code& err_code)
{
	//kick by default
	switch (err_code.value())
	{
	case error::connection_reset:
#if _LOGGING_
		Log(Action, "[%s] - Disconnected", ConnectionString(connection).c_str());
#endif // _STATE_MESSAGE
		_DeleteConnection(connection);
		break;
		
	default:
#if _LOGGING_
		Log(Mistake, "[%s] - Drop connection: #%d: %s", ConnectionString(connection).c_str(), err_code.value(), err_code.message());
#endif // _STATE_MESSAGE
		_DeleteConnection(connection);
		break;
	}
}

//check 
void Server::_DeleteConnection(Connection* connection)
{
	error_code err;
	//������������� ��� �������� ������-������ ��� ������
	connection->_Socket.shutdown(ip::tcp::socket::shutdown_both, err);
	//assert ����� ��������
	//�� ���� ��� ��������� ������������ ����� ��������
	assert(!err);

	//TODO: change this
	if ( connection->Account.Online() )
		_MessagerEngine->OnLogout(connection);

	//���� ������� � ������ �������
	auto res = find(_Connections.begin(), _Connections.end(), connection);
	//���� ����� ������� � ������ ���, ���-�� ����� �� ���
	if (res == _Connections.end())
		throw;

	//������� ������� �� ������
	_Connections.erase(res);

	//��������� �����
	connection->_Socket.close();
	delete connection;
	return;
}