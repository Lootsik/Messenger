#include "Deserialization.h"

namespace Deserialization 
{
	bool PacketCheckup(const char* paket, size_t size)
	{
		return ((_PacketMarkup*)paket)->DataSize + 4 == size;
	}

	int PaketType(const char* paket, size_t size)
	{
		return ((_PacketMarkup*)paket)->Type;
	}			

	int OnLogout()
	{
		return 0;
	}
	int OnLogin(const char* packet, size_t size, std::string& LoginFrom, std::string& PassFrom)
	{
		//TODO: hide this 4
		if (size < 4)
			return (int)Result::WrongSizing;



		_PacketMarkup* Packet = (_PacketMarkup*)(packet);
		_LoginMarkup* LoginInfo = (_LoginMarkup*)( Packet->Data );

		uint16_t SizeLogin = LoginInfo->LoginSize;
		uint16_t SizePass = LoginInfo->PassSize;
		
		//size of login and pass size
		if ( 4 + SizePass + SizeLogin != Packet->DataSize)
			return (int)Result::WrongSizing;

		//CHECK: end can be not in
		LoginFrom = std::string{ LoginInfo->Data , LoginInfo->Data + SizeLogin  };
		PassFrom = std::string{ LoginInfo->Data + SizeLogin , LoginInfo->Data + SizeLogin + SizePass  };

		return (int)Result::Ok;
	}
}