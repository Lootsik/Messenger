#pragma once
#include "PacketFormat.h"
#include <string>
namespace Serialization 
{

	enum class Result
	{
		Ok,
		WrongSizing
	};

	//size of packet data must be MaxPacketLenght
	//login and pass with last zero byte
	int MakePacketLogin(char* packet, const  std::string& GuessLogin, const std::string& GuessPass);
	int MakePacketLogout(char* packet);
}
