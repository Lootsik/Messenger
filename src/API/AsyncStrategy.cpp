#include <memory>

#include <Protocol\BaseHeader.h>
#include <Protocol\GlobalInfo.h>
#include <Protocol\TransferredData.h>

#include "AsyncStrategy.h"


using namespace boost::asio;

static bool ValidSize(size_t size) {
	return size >= BaseHeader::HeaderSize() && size <= PacketMaxSize;
}

static bool NeedReadMore(size_t size)
{
	return size	> BaseHeader::HeaderSize();
}

// already know that received header
static size_t BytesToReceive(size_t FrameSize)
{
	return FrameSize - BaseHeader::HeaderSize();
}


/*
	****************************************
					Core
	****************************************
*/

AsyncStrategy::AsyncStrategy(PacketAnalyzer analyzer, MessengerAPI& api)
	:Analyzer{ analyzer }, API{api},
	Worker{ [&]() {
				_BindTimer();
				service.run();
		} },  _ThreadWorking{ true }
{
}

AsyncStrategy::~AsyncStrategy()
{
	service.stop();
//	Worker.detach();
}

void AsyncStrategy::Reset()
{
	_Disconnect();
	//delete all packets 
	while (true)
	{
		try {
			auto p = GetPacket();
			delete p;
		}
		catch (...){
			break;
		}
	}
}

void AsyncStrategy::_CheckPacket(const Byte* packet, size_t size)
{
	auto Data = Analyzer.Analyze(packet, size);
	// deserialization fail
	if (Data == nullptr)
		return;

	// if false, user need to see this
	if (! API.Process(Data))
	{
		query.push_back(Data);
	}
	else {
		delete Data;
	}
}


void AsyncStrategy::_SolveProblem(const boost::system::error_code& err_code)
{
	auto s = err_code.message();
	
	switch (err_code.value()) {

	case 2:
		if (_Connected) {
			sock.shutdown(socket_base::shutdown_both);
			sock.close();
			_Connected = false;
		}break;
	}
}

void AsyncStrategy::_Disconnect()
{
	if (_Connected) {
		sock.shutdown(socket_base::shutdown_both);
		sock.close();
		_Connected = false;
	}
}

/*
	***********************************************
					Bind message
	***********************************************
*/

void AsyncStrategy::_BindMessage()
{
	boost::asio::async_read(sock, buffer(ReadBuff, BaseHeader::HeaderSize()),
			boost::bind(&AsyncStrategy::_AcceptMessage, this, _1, _2));
}



void AsyncStrategy::_BindMessageRemainder(size_t size)
{
	boost::asio::async_read(sock, buffer(
			ReadBuff.c_array() + BaseHeader::HeaderSize(), size),
			boost::bind(&AsyncStrategy::_AcceptMessageRemainder, this, _1, _2));
}





/*
	***********************************************
				Accept message callbacks
	***********************************************
*/



void AsyncStrategy::_AcceptMessage(const boost::system::error_code& err_code, size_t bytes)
{
	//TODO: do smth
	if (err_code)
	{
		_SolveProblem(err_code);
		return;
	}


	size_t FrameSize = BaseHeader::FrameSize(ReadBuff.c_array());

	//TODO: do smth
	if (!ValidSize(FrameSize))
	{
		_BindMessage();
		return;
	}

	// if we packet not fully transferred
	if (NeedReadMore(FrameSize))
	{
		_BindMessageRemainder(BytesToReceive(FrameSize));
		return;
	}

	_CheckPacket(ReadBuff.c_array(), bytes);
	
	_BindMessage();
}


void AsyncStrategy::_AcceptMessageRemainder(const boost::system::error_code& err_code, size_t bytes)
{
	if (err_code)
	{
		_SolveProblem(err_code);
		return;
	}
	_CheckPacket(ReadBuff.c_array(), bytes + BaseHeader::HeaderSize());

	_BindMessage();
}

BaseHeader* AsyncStrategy::GetPacket()
{
	return query.pop_front();
}


/*
	***********************************************
	Accept message callbacks
	***********************************************
*/



bool AsyncStrategy::Connect(const std::string& Address, unsigned short port)
{
	return Connect( boost::asio::ip::tcp::endpoint { ip::address::from_string(Address), port });
}

bool AsyncStrategy::Connect(const boost::asio::ip::tcp::endpoint& endpoint)
{
	ep = endpoint;

	boost::system::error_code err;

	sock.connect(ep, err);
	
	if (err)
		//connection can be refuzed
		throw ConnectionTrouble{};

	_BindMessage();
	
	_Connected = true;

	return true;
}


bool AsyncStrategy::Ready()
{
	return query.ready();
}



/*
	***********************************************
						Send
	***********************************************
*/



bool AsyncStrategy::Send(const TransferredData& Data)
{
	if (!_Connected)
	{
		try {
			Connect(ep);
		}
		catch (ConnectionTrouble){
			throw;
		}
	}

	if (Data.NeededSize() > WriteBuff.size())
		return false;

	int ser_err = Data.ToBuffer(WriteBuff.c_array());

	if (ser_err)
		return false;


	sock.async_write_some(
			buffer(WriteBuff, Data.NeededSize()),
			boost::bind(&AsyncStrategy::_WriteHandler, this, _1, _2));

	return true;
}

void AsyncStrategy::_WriteHandler(const boost::system::error_code& err, size_t bytes)
{
	if (err)
		throw;
}




/*
	***********************************************
						Timer
	***********************************************
*/


void AsyncStrategy::_BindTimer()
{
	_Timer.expires_from_now(boost::posix_time::millisec(30000));
	_Timer.async_wait(boost::bind(&AsyncStrategy::_DumpTimer, this,_1));
}

void AsyncStrategy::_DumpTimer(const boost::system::error_code& error)
{
	_BindTimer();
}