#pragma once
#include <string>
#include <boost\asio.hpp>
#include <boost\array.hpp>
#include <stdint.h>
#include <GlobalInfo.h>

struct AccountInfo
{
	//only this for now
	uint32_t ID = INVALID_ID;

	inline bool Online() {
		return ID != INVALID_ID;
	}
};

struct Connection {
	Connection(boost::asio::io_service& service) :_Socket{ service } {}
	static const size_t Buffsize = PacketMaxSize;
	using Storage = boost::array<Byte, Buffsize>;

	Byte* ReadBuf()
	{
		return _ReadBuff.data();
	}
	Byte* WriteBuf()
	{
		return _WriteBuf.data();
	}
	//Data
	size_t BytesRead;
	Storage _ReadBuff;

	size_t BytesWrite;
	Storage _WriteBuf;
	
	boost::asio::ip::tcp::socket _Socket;
	AccountInfo Account;
	boost::posix_time::ptime LastSeen;
};
