#pragma once
#include <stdint.h>

//standart 2 byte uinteger used
//little endian for now 
//TODO: do smth with this

enum class PacketTypes{
	Message = 3,
	Login = 4,
	Logout = 5
};

//core of packet, in Data contained some data	
struct _PacketMarkup
{
	//will be necessery to add some field there later
	uint16_t Type;
	//Only data field size!
	uint16_t DataSize;
	char Data[1];
};

struct _LoginMarkup 
{
	uint16_t LoginSize;
	uint16_t PassSize;
	char Data[1];
};
struct _LogoutMarkup
{
	
};

const size_t MaxPacketLenght = 256;
const size_t MaxDataSize = MaxPacketLenght
	- sizeof(_PacketMarkup) - sizeof(_PacketMarkup::Data);
