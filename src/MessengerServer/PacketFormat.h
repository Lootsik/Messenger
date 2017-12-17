#include <stdint.h>

//standart 2 byte uinteger used
//little endian for now 
//TODO: do smth with this

enum class PacketType {
	Message = 3,
	Login = 4,
	Logout = 5
};

//core of packet, in Data contained some data	
struct _PacketMarkup
{
	//will be necessery to add some field there later
	//size of all packet
	uint16_t Size;
	uint16_t Type;
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
