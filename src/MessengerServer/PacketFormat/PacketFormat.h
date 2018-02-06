#pragma once
#include <GlobalInfo.h>

//TODO: manage namespaces
namespace Packet {
	//little endian for now 
	//TODO: make portable to big endian 
#pragma warning( push )
#pragma warning( disable : 4200 )
// Your function
//will be good idea close up this big names with namespaces 
	//vision
	enum class Types{
		Message = 3,
		Login = 4,
		LoginResult = 5,
		Logout = 6,
		UncheckedEvent
	};

	//take care of aligment

	//core of packet, in Data contained some data	
	struct Header
	{
		//will be necessery to add some field there later
		uint16_t Type;
		//Only data field size!
		uint16_t DataSize;
		alignas(4) BYTE Data[0];
	};

	const size_t MaxSize = ::PacketMaxSize;
	//its minimal size for packet for now
	const size_t HeaderSize = sizeof(Header);
	const size_t MaxDataSize = MaxSize - HeaderSize;

	//later it can be useful

	namespace Login {
		struct Header
		{
			uint16_t LoginSize;
			uint16_t PassSize;
			alignas(4) BYTE Data[0];
		};
		//its without packet header itself
		const size_t HeaderSize = sizeof(Header);
		const size_t MaxDataSize = Packet::MaxDataSize - HeaderSize;
	}
	namespace Messagee {
		struct Header
		{
			uint32_t IdFrom;
			uint32_t IdTo;
			uint32_t Index;
			uint16_t MessageSize;
			alignas(4) BYTE Data[0];
		};
		const size_t HeaderSize = sizeof(Header);
		const size_t MaxDataSize = Packet::MaxDataSize - HeaderSize;
	}
#pragma warning( pop ) 

	namespace Logout {
		struct Header
		{
		};
	}

	namespace LoginResult
	{
		enum Result
		{
			Success,
			Wrong,
			//account already online
			AccInUse,
			//connected already online
			AlreadyOnline,
			NoSuchUser
		};

		struct Header
		{
			uint32_t Result;
			uint32_t ID;
		};

		const size_t PacketHeaderSize = sizeof(Header);
	}
	namespace UncheckedEvent
	{

	}
}
