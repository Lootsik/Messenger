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

//TODO: rewrite to blocking mode

using namespace boost::asio;
using boost::system::error_code;
using namespace Logger;
//TODO: add some error checking

//todo add ID
std::string ConnectionString(const Connection* connection)
{
	if(connection->_Socket.is_open())
		return connection->_Socket.remote_endpoint().address().to_string() +
					":" + std::to_string(connection->_Socket.remote_endpoint().port());
	
	return "";
}


//*****************************
//	Network settings
//*****************************

Network::Network(boost::asio::io_service& service)
	: _Service{ service },
		_Acceptor{ service },
		_Timer{ service } {}


//TODO: rewrite this
//NOTE: add new settings to config
bool Network::LoadFromConfig(const char* ConfigFilename)
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
	connection->LastSeen = boost::posix_time::microsec_clock::local_time();
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
	_Acceptor.async_accept(new_Connection->_Socket, boost::bind(&Network::_AcceptConnections, this, new_Connection, _1), error);

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
	connection->_Socket.async_write_some(
				buffer(connection->_WriteBuf, connection->BytesWrite),
				boost::bind(&Network::_WriteHandler, this, _1, _2));
}

//*******************************
//	Accept message/cliet handlers
//*******************************


void Network::_AcceptConnections(PConnection connection, const boost::system::error_code& err)
{
	//добавление асинхронной операции для принятого клиента(страшно, нужно упростить мб вывести в другую функцию)
	connection->_Socket.async_read_some(buffer(connection->_ReadBuff), boost::bind(&Network::_AcceptMessage, this, connection, _1, _2));
	//добавление клиента в список (не  критично, тк вызовы для сокета уже забинджены)

#if _LOGGING_
	Log(Action, "New connection: %s:%d",connection->_Socket.remote_endpoint().address().to_string().c_str(),
										 connection->_Socket.remote_endpoint().port());
#endif

	_Connections.push_back(connection);

	//создаём нового клиента
	boost::shared_ptr<Connection> new_Connection{ new Connection{ _Service } };

	error_code error;
	//регистрируем обработчик нового коннекта
	_LastSeenNow(connection);

	_Acceptor.async_accept(new_Connection->_Socket, boost::bind(&Network::_AcceptConnections, this, new_Connection, _1), error);

	//TODO: fix this
	if (error)
		throw;
}

void Network::_OnTimer()
{
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	if (_Connections.size() != 0) 
	{
		size_t i = _Connections.size();
		while(i!=0)
		{
			--i;
			if ((now - _Connections[i]->LastSeen).total_milliseconds() > Timeout)
			{
				_DeleteConnection(_Connections[i]);
			}
		}
	}
	_BindTimer();
}

void Network::_BindTimer()
{
	_Timer.expires_from_now(boost::posix_time::millisec(Timeout));
	_Timer.async_wait(boost::bind(&Network::_OnTimer, this));
}

//TODO: rewrite this
void  Network::_AcceptMessage(PConnection connection, const boost::system::error_code& err_code, size_t bytes)
{
	//ошибка, нужно удалить клиента
	if (err_code) {
		//TODO: стоит определить план дейстивий для конкретного случая: принимать дальше сообщения или нет
		_SolveProblemWithConnection(connection, err_code);
		return;
	}

	_LastSeenNow(connection);

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
									boost::bind(&Network::_AcceptMessage,this, connection, _1, _2) );

	connection->BytesRead = bytes;
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
#if _LOGGING_
		Log(Action, "[%s] - Disconnected", ConnectionString(connection.get()).c_str());
#endif // _STATE_MESSAGE
		_DeleteConnection(connection);
		break;

	case error::operation_aborted:
		break;

	default:
#if _LOGGING_
		Log(Mistake, "[%s] - Drop connection: #%d: %s", ConnectionString(connection.get()).c_str(), err_code.value(), err_code.message());
#endif // _STATE_MESSAGE
		_DeleteConnection(connection);
		break;
	}
}

//check 
void Network::_DeleteConnection(PConnection& connection)
{
	error_code err;
	if (connection->_Socket.is_open())
	{
		//останавливаем все операции чтения-записи для сокета
		try
		{
			connection->_Socket.shutdown(ip::tcp::socket::shutdown_both);
			connection->_Socket.close();
		}
		catch (...)
		{
			Log(Error, "SAD");
		}
	}
	//assert(!err);

	//TODO: change this
	if ( connection->Account.Online() )
		_MessagerEngine->OnLogout(connection);

	//ищем клиента в списке клиетов
	auto res = find(_Connections.begin(), _Connections.end(), connection);
	//если этого клиента в списке нет, что-то пошло не так
	if (res == _Connections.end())
		return;

	//удаляем клиента со списка
	_Connections.erase(res);

	//закрываем сокет
	connection->_Socket.close();

	return;
}