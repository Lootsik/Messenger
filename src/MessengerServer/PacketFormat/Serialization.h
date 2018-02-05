#pragma once
#include "PacketFormat.h"
#include <string>
namespace Serialization 
{

	enum Result
	{
		Ok,
		WrongSizing,
		TooBigSize
	};
	//size of created packet
	size_t CountSize(BYTE* packet);
	//size of packet data must be MaxPacketLenght
	int MakePacketLogin(BYTE* packet, const  std::string& GuessLogin, const std::string& GuessPass);
	int MakePacketLogout(BYTE* packet);
	int MakePacketMessage(BYTE* packet, uint32_t from, uint32_t to , uint32_t index, const BYTE* message, uint16_t MessageSize);
	int MakePacketUncheckedEvent(BYTE* packet);

	int MakePacketLoginResult(BYTE* packet, const uint32_t result, const uint32_t ID = 0);
}
