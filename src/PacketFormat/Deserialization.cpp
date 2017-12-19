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
		if (size < PacketMarkupHeaderSize)
			return (int)Result::WrongSizing;


		_PacketMarkup* Packet = (_PacketMarkup*)packet;
		_LoginMarkup* LoginInfo = (_LoginMarkup*)( Packet->Data );

		uint16_t SizeLogin = LoginInfo->LoginSize;
		uint16_t SizePass = LoginInfo->PassSize;
		
		//size of login and pass size
		if (LoginPacketHeaderSize + SizePass + SizeLogin != Packet->DataSize)
			return (int)Result::WrongSizing;

		//CHECK: end can be not in
		LoginFrom = std::string{ LoginInfo->Data , LoginInfo->Data + SizeLogin  };
		PassFrom = std::string{ LoginInfo->Data + SizeLogin , LoginInfo->Data + SizeLogin + SizePass  };

		return (int)Result::Ok;
	}
	int OnMessage(const char* packet, size_t size, uint32_t& from, uint32_t& to, uint32_t& MessageSize, char*& Message)
	{
		_PacketMarkup* Packet = (_PacketMarkup*)packet;

		_MessageMarkup* MessageHeader = (_MessageMarkup*)(Packet->Data);

		from = MessageHeader->IdFrom;
		to = MessageHeader->IdTo ;
		MessageSize = MessageHeader->MessageSize;

		if (MessageSize + MessagePacketHeaderSize != Packet->DataSize)
			return (int)Result::WrongSizing;

		Message = MessageHeader->Data;
		return (int)Result::Ok;
	}
}