#pragma once
#include <stdint.h>

//TODO: manage namespaces
namespace Packet {
	//little endian for now 
	//TODO: make portable to big endian 
#pragma warning( push )
#pragma warning( disable : 4200)
// Your function
//READABILITY: will be good idea close up this big names with namespaces 
	enum class Types {
		Message = 3,
		Login = 4,
		LoginResult = 5,
		Logout = 6
	};
	//take care of aligment

	//core of packet, in Data contained some data	
	struct Header
	{
		//will be necessery to add some field there later
		uint16_t Type;
		//Only data field size!
		uint16_t DataSize;
		char Data[0];
	};

	const size_t MaxSize = 512;
	//its minimal size for packet for now
	const size_t HeaderSize = sizeof(Header);
	const size_t MaxDataSize = MaxSize - HeaderSize;

	//later it can be useful
#if Packet::MaxSize > UINT16_MAX
#error "Max packet size larger that max value that may contained it"
#endif
	namespace Login {
		struct Header
		{
			uint16_t LoginSize;
			uint16_t PassSize;
			char Data[0];
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
			uint16_t MessageSize;
			uint16_t _unused;
			char Data[0];
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
		enum class Result
		{
			Success,
			Wrong,
			NoSuchUser
		};

		struct Header
		{
			uint32_t Result;
			uint32_t ID;
		};

		const size_t PacketHeaderSize = sizeof(Header);
	}
}