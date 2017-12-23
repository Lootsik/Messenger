#pragma once
#include "PacketFormat.h"
#include <string>
namespace Serialization 
{

	enum class Result
	{
		Ok,
		WrongSizing,
		TooBigSize
	};
	//size of created packet
	size_t CountSize(char* packet);
	//size of packet data must be MaxPacketLenght
	//login and pass with last zero byte
	int MakePacketLogin(char* packet, const  std::string& GuessLogin, const std::string& GuessPass);
	int MakePacketLogout(char* packet);
	int MakePacketMessage(char* packet, uint32_t from, uint32_t to, const char* message, uint16_t MessageSize);
	
	int MakePacketLoginResult(char* packet, const uint32_t result, const uint32_t ID = 0);
}
