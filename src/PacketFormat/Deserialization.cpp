#include "Deserialization.h"

namespace Deserialization 
{
	bool PacketCheckup(const char* paket, size_t size)
	{
		return ((_PacketMarkup*)paket)->DataSize + PacketMarkupHeaderSize == size;
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

		//becouse last - zero 
		LoginFrom = std::string{ LoginInfo->Data , LoginInfo->Data + SizeLogin - 1 };
		PassFrom = std::string{ LoginInfo->Data + SizeLogin , LoginInfo->Data + SizeLogin + SizePass -1   };

		return (int)Result::Ok;
	}
	int OnMessage(const char* packet, size_t size, uint32_t& from, uint32_t& to, uint32_t& MessageSize, char** Message)
	{
		_PacketMarkup* Packet = (_PacketMarkup*)packet;

		_MessageMarkup* MessageHeader = (_MessageMarkup*)(Packet->Data);

		from = MessageHeader->IdFrom;
		to = MessageHeader->IdTo ;
		MessageSize = MessageHeader->MessageSize;

		if (MessageSize + MessagePacketHeaderSize != Packet->DataSize)
			return (int)Result::WrongSizing;

		*Message = MessageHeader->Data;
		return (int)Result::Ok;
	}
	int OnLoginResult(const char* packet, uint32_t& result, uint32_t& id)
	{
		_PacketMarkup* Packet = (_PacketMarkup*)packet;
		LoginResult::LoginResult* Result = (LoginResult::LoginResult*)(Packet->Data);

		result = Result->Result;
		id = Result->ID;
		return (int)Result::Ok;
	}
}