#pragma once
#include <memory>

#include <boost\asio.hpp>
#include <boost\array.hpp>
#include <boost\thread.hpp>

#include <Protocol\GlobalInfo.h>
#include <Protocol\BaseHeader.h>

#include <API\MessengerAPI.h>
#include <API\NetworkStrategy.h>
#include <API\Query.h>

using PacketQuery = Query<TransferredData*>;

class AsyncStrategy :
	public NetworkStrategy
{
public:

	AsyncStrategy( MessengerAPI& api);
	virtual ~AsyncStrategy();

	bool Connect(const std::string& Address, unsigned short port) override;

	bool Ready() override;
	TransferredData* GetPacket() override;


	bool Send(const TransferredData& Data) override;
	void Release() override;

	class NetworkTrouble {
	public:
		NetworkTrouble(const wchar_t* what) : message{ what } {}
		const wchar_t* what()const { return message; }
	private:
		const wchar_t* message;
	};


private:
	bool _Dropped = false;

	MessengerAPI & API;

	//analyze packet, and push to query if needed
	void _CheckPacket(const Byte* packet, size_t size);


	void _WriteHandler(const boost::system::error_code& err, size_t bytes);

	bool Connect(const boost::asio::ip::tcp::endpoint& endpoint);

	// bind callbacks
	void _BindMessage();
	void _BindMessageRemainder(size_t size);

	// callbacks 
	void _AcceptMessage(const boost::system::error_code& err_code, size_t bytes);
	void _AcceptMessageRemainder(const boost::system::error_code& err_code, size_t bytes);

	// to keep io_service working
	void _BindTimer();
	void _DumpTimer(const boost::system::error_code& error);

	void _SolveProblem(const boost::system::error_code& err_code);

	void _Disconnect();
	/*
		Data 
	*/
	PacketQuery query;

	boost::array <Byte, PacketMaxSize> WriteBuff;
	boost::array <Byte, PacketMaxSize> ReadBuff;

	boost::asio::io_service service;
	boost::asio::ip::tcp::endpoint ep;
	boost::asio::ip::tcp::socket sock{ service };

	boost::asio::deadline_timer _Timer{ service };
	
	boost::thread Worker;
	
	bool _ThreadWorking = false;
	bool _Connected = false;
};

