#pragma once
#include <boost\thread\mutex.hpp>
#include <boost\asio.hpp>
#include <boost\thread.hpp>
#include "../PacketFormat/Serialization.h"
#include "../PacketFormat/Deserialization.h"

//TODO: some error handling 

using namespace boost::asio;
class ClientNetwork
{
public:
	ClientNetwork();
	~ClientNetwork();

	int Connect(const ip::address& address, unsigned short port)
	{
		ep = ip::tcp::endpoint{ address, port };

		boost::system::error_code err;
		sock.connect(ep, err);
		return !err;
	}
	void TryLogin(const std::string& Login, const std::string& Pass)
	{
		Serialization::MakePacketLogin(buff, Login, Pass);
		boost::system::error_code err;
		sock.write_some(boost::asio::buffer(buff, Serialization::CountSize(buff)), err);
		err;
	}
	bool WaitResponse()
	{
		boost::system::error_code err;
		size_t readed = sock.read_some(buffer(readbuff, 512), err);
		if (err) {
			//Log
			return false;
		}
		uint32_t res;
		uint32_t id;
		Deserialization::OnLoginResult(readbuff, res, id);
		ID = id;
		return  res == (int)LoginResult::Result::Success;
	}

	bool Authentication(const std::string& Login, const std::string& Pass)
	{
		TryLogin(Login, Pass);
		return  WaitResponse();
	}

	void StartAcceptMessages()
	{
		reader = boost::thread{ &ClientNetwork::InfRead, this };
		//Did we need detach?
		reader.detach();
	}

	void Message(uint32_t to, char* message, size_t size)
	{
		int res = Serialization::MakePacketMessage(buff, ID, to, message, size);
		if (res != (int)Serialization::Result::Ok)
		{
			throw;
			return;
		}
		boost::system::error_code err;

		write(sock, buffer(buff, Serialization::CountSize(buff)), err);
		err;
	}
	void InfRead()
	{
		while (1) {
			boost::system::error_code err;
			size_t readed = sock.read_some(buffer(readbuff, 512),err);
			err;
			if (err)
				return;

			boost::recursive_mutex::scoped_lock look{ WaitMutex };

			memcpy(WaitBuff, readbuff, readed);
			size = readed;
			ReadyToRead = true;
		}
	}
	

	bool ReadyToRead = false;
	char WaitBuff[512];
	size_t size;
		

	boost::thread reader;
	char buff[512];
	char readbuff[512];
	io_service service;
	uint32_t ID;
	ip::tcp::endpoint ep;
	ip::tcp::socket sock{ service };
	boost::recursive_mutex WaitMutex;
};

