#include "Serialization.h"

namespace Serialization
{
	size_t CountSize(char* packet)
	{
		return ((Packet::Header*)packet)->DataSize + Packet::HeaderSize;
	}

	int MakePacketLogin(char* packet,const std::string& GuessLogin,const std::string& GuessPass)
	{
		size_t LoginSize = GuessLogin.size();
		size_t PassSize = GuessPass.size();
		
		//DANGER: possible owerflow
		size_t PacketDataSize = Packet::Login::HeaderSize + LoginSize + PassSize;

		if (PacketDataSize > Packet::Login::MaxDataSize)
			//size of string is too big for packet
			return (int)Result::TooBigSize;

		Packet::Header* CurPacket = (Packet::Header*) packet;

		CurPacket->Type = (uint16_t) Packet::Types::Login;
		//its safe, becouse DataSize awlays larger that MaxLoginPacketDataSize 
		CurPacket->DataSize = PacketDataSize;

		//working with Login Header
		Packet::Login::Header* LoginInfo = (Packet::Login::Header*)(CurPacket->Data);
	
		LoginInfo->LoginSize = LoginSize;
		LoginInfo->PassSize = PassSize;
		//copying string
		memcpy(LoginInfo->Data, GuessLogin.c_str(), LoginInfo->LoginSize );
		memcpy(LoginInfo->Data + LoginInfo->LoginSize , GuessPass.c_str(), LoginInfo->PassSize );

		return 0;
	}
	int MakePacketLogout(char* packet)
	{
		((Packet::Header*)packet)->Type = (uint16_t)Packet::Types::Logout;
		((Packet::Header*)packet)->DataSize = 0;
		return 0;
	}
	int MakePacketMessage(char* packet, uint32_t from,uint32_t to, const char* message, uint16_t MessageSize)
	{
		int MessageDataSize = Packet::Messagee::HeaderSize + MessageSize;
		if (MessageDataSize > Packet::Messagee::MaxDataSize) {
			return (int)Result::TooBigSize;
		}
		Packet::Header* Packet = (Packet::Header*)packet;

		Packet->Type = (uint16_t)Packet::Types::Message;
		Packet->DataSize = MessageDataSize;

		Packet::Messagee::Header* MessageHeader = (Packet::Messagee::Header*) (Packet->Data);

		MessageHeader->IdFrom = from;
		MessageHeader->IdTo = to;
		MessageHeader->MessageSize = MessageSize;

		memcpy(MessageHeader->Data, message, MessageSize);
		return (int)Result::Ok;
	}
	//if result is not 
	int MakePacketLoginResult(char* packet, const uint32_t result, const uint32_t ID )
	{

		Packet::Header* Packet = (Packet::Header*)packet;

		Packet->Type = (uint16_t)Packet::Types::LoginResult;
		Packet->DataSize = Packet::LoginResult::PacketHeaderSize;

		Packet::LoginResult::Header* ResultHeader = (Packet::LoginResult::Header*)(Packet->Data);

		ResultHeader->Result = result;
		ResultHeader->ID = ID;
		return (int)Result::Ok;
	}
}
