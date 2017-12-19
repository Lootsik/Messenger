#pragma once
#include <stdint.h>

//standart 2 byte uinteger used
//little endian for now 
//TODO: make portable to big endian 

//READABILITY: will be good idea close up this big names with namespaces 
enum class PacketTypes{
	Message = 3,
	Login = 4,
	Logout = 5
};
//take care of aligment

//core of packet, in Data contained some data	
struct _PacketMarkup
{
	//will be necessery to add some field there later
	uint16_t Type;
	//Only data field size!
	uint16_t DataSize;
	char Data[1];
};

const size_t MaxPacketSize = 512;
//its minimal size for packet for now
const size_t PacketMarkupHeaderSize = sizeof(_PacketMarkup) - sizeof(_PacketMarkup::Data);
const size_t MaxPacketDataSize = MaxPacketSize - PacketMarkupHeaderSize;

//later it can be useful
#if MaxPacketSize > UINT16_MAX
	#error "Max packet size larger that max value that may contained it"
#endif

struct _LoginMarkup 
{
	uint16_t LoginSize;
	uint16_t PassSize;
	char Data[0];
};

//its without packet header itself
const size_t LoginPacketHeaderSize = sizeof( uint16_t) * 2;
const size_t MaxLoginPacketDataSize = MaxPacketDataSize - LoginPacketHeaderSize;

struct _MessageMarkup 
{
	uint32_t IdFrom;
	uint32_t IdTo;
	uint16_t MessageSize;
	char Data[0];
};
const size_t MessagePacketHeaderSize = 10;
const size_t MaxMessageSize = MaxPacketDataSize - MessagePacketHeaderSize;



struct _LogoutMarkup
{
};
