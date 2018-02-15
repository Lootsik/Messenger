#pragma once
#include <string>
#include <boost\asio.hpp>
#include <boost\array.hpp>
#include <boost\shared_ptr.hpp>

#include <GlobalInfo.h>

class AccountInfo
{
public:
	AccountInfo(uint32_t id = INVALID_ID) : _ID{ id } {}

	bool Online() const {	return _ID != INVALID_ID;}
	ID_t ID() const { return _ID; }
	void SetID(ID_t ID) { _ID = ID; }
	void Reset() { _ID = INVALID_ID; }
private:
	ID_t _ID = INVALID_ID;
};

class Connection {
public:
	Connection(boost::asio::io_service& service) :_Socket{ service } {}

	static const size_t Buffsize = PacketMaxSize;
	using Storage = boost::array<Byte, Buffsize>;
			
	//				Getters, setters

	/*******************************************/
	void SetBytesToRead(const size_t bytes) { _BytesToRead = bytes; }
	void SetBytesToWrite(const size_t bytes) { _BytesToWrite = bytes; }
	/*******************************************/
	size_t BytesToRead()const { return _BytesToRead; }
	size_t BytesToWrite()const { return _BytesToWrite; }
	/*******************************************/
	Storage& ReadBuffer() { return _ReadBuff; }
	Storage& WriteBuffer() { return _WriteBuff; }

	//					Time
	boost::posix_time::ptime& LastTime() { return _LastSeen; }
	AccountInfo& Account() { return _Account; }
	const boost::asio::ip::tcp::socket& Socket() const { return _Socket; }
	boost::asio::ip::tcp::socket& Socket() { return _Socket; }

	std::string ConnectionString() const 
	{
		if (_Account.Online())
			return "id:" + std::to_string(_Account.ID());

		if (_Socket.is_open())
			return _Socket.remote_endpoint().address().to_string() +
				":" + std::to_string(_Socket.remote_endpoint().port());

		return "INVALID CONNECTION";
	}

private:
	//		Read info
	size_t _BytesToRead;
	Storage _ReadBuff;

	//		Write info
	size_t _BytesToWrite;
	Storage _WriteBuff;

	boost::asio::ip::tcp::socket _Socket;
	boost::posix_time::ptime _LastSeen;
	AccountInfo _Account;
};

using PConnection = boost::shared_ptr<Connection>;
