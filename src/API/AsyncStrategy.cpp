#include <memory>

#include <Protocol\BaseHeader.h>
#include <Protocol\GlobalInfo.h>
#include <Protocol\TransferredData.h>
#include <Protocol\Types.h>
#include <Protocol\SerializationError.h>

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

AsyncStrategy::AsyncStrategy( MessengerAPI& api)
				: API{api},
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

void AsyncStrategy::Release()
{
	_Disconnect();
	_Dropped = false;
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
	TransferredData* Transferred{};
	if (Types::FromBuffer(packet, size, Transferred) != SerializationError::Ok)
		return;


	// if false, user need to see this
	if (! API.Process(Transferred))
	{
		query.push_back(Transferred);
	}
	else {
		delete Transferred;
	}
}

// TODO: more
void AsyncStrategy::_SolveProblem(const boost::system::error_code& err_code)
{
	auto s = err_code.message();
	
	switch (err_code.value()) {

	case 2:
		_Disconnect();
		break;

	case 10054:
	{
		// connection dropped
		_Disconnect();
		_Dropped = true;
	}break;

		
	case 10061:
	{
		// connection refused
	}break;
	default:
		_Disconnect();
		_Dropped = true;
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

TransferredData* AsyncStrategy::GetPacket()
{
	try
	{
		return query.pop_front();
	}
	catch (...)
	{
		throw;
	}
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
	//TODO: mb connect to another ep
	if (_Connected && endpoint == ep)
		return true;
	else if (endpoint == ep)
	{
		_Disconnect();
	}

	ep = endpoint;

	boost::system::error_code err;
	sock.connect(ep, err);
	
	if (err)
		throw __ConnectionRefused{};

	_BindMessage();
	_Connected = true;

	return true;
}


bool AsyncStrategy::Ready()
{
	if (_Dropped)
		throw  __ConnectionDropped{};

	return query.ready();
}



/*
	***********************************************
						Send
	***********************************************
*/



bool AsyncStrategy::Send(const TransferredData& Data)
{
	// connection dropped exception will dropped soon
	
	if (!_Connected)
	{
		try {
			Connect(ep);
		}
		catch (__ConnectionRefused){
			if (_Dropped)
				return false;
			else
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
		_SolveProblem(err);
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