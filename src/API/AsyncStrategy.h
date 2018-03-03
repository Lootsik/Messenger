#pragma once
#include <boost\asio.hpp>
#include <boost\array.hpp>
#include <boost\thread.hpp>

#include "NetworkStrategy.h"
#include <Protocol\GlobalInfo.h>
#include <Protocol\BaseHeader.h>

#include <memory>

#include "Query.h"

#include <API\PacketAnalyzer.h>
#include <API\MessengerAPI.h>

using PacketQuery = Query<std::shared_ptr<BaseHeader>>;

class AsyncStrategy :
	public NetworkStrategy
{
public:
	AsyncStrategy(PacketAnalyzer analyzer, MessengerAPI& api);

	bool Connect(const std::string& Address, unsigned short port) override;

	bool Ready() override;
	std::shared_ptr<TransferredData> GetPacket() override;
	bool Send(const TransferredData& Data) override;

	//bool Connected 
	virtual ~AsyncStrategy();

	class ConnectionTrouble{};

private:

	MessengerAPI & API;

	void _CheckPacket(const Byte* packet, size_t size);


	void _WriteHandler(const boost::system::error_code& err, size_t bytes);


	bool Connect(const boost::asio::ip::tcp::endpoint& endpoint);

	void _BindMessage();
	void _BindMessageRemainder(size_t size);

	void _AcceptMessage(const boost::system::error_code& err_code, size_t bytes);
	void _AcceptMessageRemainder(const boost::system::error_code& err_code, size_t bytes);


	PacketQuery query;
	PacketAnalyzer Analyzer;

	boost::thread Worker;

	boost::array <Byte, PacketMaxSize> WriteBuff;
	boost::array <Byte, PacketMaxSize> ReadBuff;

	boost::asio::io_service service;
	boost::asio::ip::tcp::endpoint ep;
	boost::asio::ip::tcp::socket sock{ service };
	bool _Connected = false;
};

