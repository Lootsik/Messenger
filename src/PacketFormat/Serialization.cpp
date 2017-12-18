#include "Serialization.h"

namespace Serialization
{
	//strings with last zero byte
	int MakePacketLogin(char* packet,const std::string& GuessLogin,const std::string& GuessPass)
	{
		//with zero terminators 
		size_t LoginSize = GuessLogin.size() + 1;
		size_t PassSize = GuessPass.size() + 1;


		//DANGER: possible owerflow
		size_t PacketDataSize = LoginPacketHeaderSize + LoginSize + PassSize;

		if (PacketDataSize > MaxLoginPacketDataSize)
			//size of string is too big for packet
			return (int)Result::StringTooLong;

		//setup type
		((_PacketMarkup*)packet)->Type = (uint16_t) PacketTypes::Login;
		//is safe, becouse DataSize awlays larger that MaxLoginPacketDataSize 
		((_PacketMarkup*)packet)->DataSize = PacketDataSize;

		//working with LoginMarkup
		_LoginMarkup* LoginInfo = (_LoginMarkup*)(((_PacketMarkup*)packet)->Data);
	
		LoginInfo->LoginSize = LoginSize;
		LoginInfo->PassSize = PassSize;
		//copying string
		memcpy(LoginInfo->Data, GuessLogin.c_str(), LoginInfo->LoginSize );
		memcpy(LoginInfo->Data + LoginInfo->LoginSize , GuessPass.c_str(), LoginInfo->PassSize );

		return 0;
	}
	int MakePacketLogout(char* packet)
	{
		((_PacketMarkup*)packet)->Type = (uint16_t)PacketTypes::Logout;
		((_PacketMarkup*)packet)->DataSize = 0;
		return 0;
	}
}

