#include "Deserialization.h"

namespace Deserialization 
{
	bool PacketCheckup(const BYTE* packet, size_t size)
	{
		return ((Packet::Header*)packet)->DataSize + Packet::HeaderSize == size;
	}

	int PaketType(const BYTE* packet, size_t size)
	{
		return ((Packet::Header*)packet)->Type;
	}			

	int OnLogout(const BYTE* packet, size_t size)
	{
		return Result::Ok;
	}

	int OnLogin(const BYTE* packet, size_t size, std::string& LoginFrom, std::string& PassFrom)
	{
		if (size < Packet::HeaderSize + Packet::Login::HeaderSize)
			return Result::WrongSizing;
		//Header
		Packet::Header* Packet = (Packet::Header*)packet;
		//LoginHeader
		Packet::Login::Header* LoginInfo = (Packet::Login::Header*)( Packet->Data );

		uint16_t SizeLogin = LoginInfo->LoginSize;
		uint16_t SizePass = LoginInfo->PassSize;
		size_t DataSize = Packet::Login::HeaderSize + SizeLogin + SizePass;
		//Check sizes
		if (DataSize != Packet->DataSize)
			return Result::WrongSizing;

		//becouse last - zero 
		LoginFrom = std::string{ LoginInfo->Data , LoginInfo->Data + SizeLogin };
		PassFrom = std::string{ LoginInfo->Data + SizeLogin , LoginInfo->Data + SizeLogin + SizePass };

		return Result::Ok;
	}
	int OnMessage(const BYTE* packet, size_t size, uint32_t& from, uint32_t& to,uint32_t& index, uint32_t& MessageSize, BYTE** Message)
	{
		Packet::Header* Packet = (Packet::Header*)packet;

		Packet::Messagee::Header* MessageHeader = (Packet::Messagee::Header*)(Packet->Data);

		from = MessageHeader->IdFrom;
		to = MessageHeader->IdTo;
		index = MessageHeader->Index;
		MessageSize = MessageHeader->MessageSize;

		if (Packet::Messagee::HeaderSize + MessageSize  != Packet->DataSize)
			return (int)Result::WrongSizing;

		*Message = MessageHeader->Data;
		return Result::Ok;
	}
	int OnLoginResult(const BYTE* packet, uint32_t& result, uint32_t& id)
	{
		Packet::Header* Packet = (Packet::Header*)packet;
		Packet::LoginResult::Header* Result = (Packet::LoginResult::Header*)(Packet->Data);

		result = Result->Result;
		id = Result->ID;
		return Result::Ok;
	}
}