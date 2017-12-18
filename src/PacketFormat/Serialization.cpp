#include "Serialization.h"

namespace Serialization
{
	//strings with last zero byte
	int MakePacketLogin(char* packet,const  std::string& GuessLogin,const std::string& GuessPass)
	{
		size_t DataSize = 4 + GuessLogin.size() + GuessPass.size() + 2;

		if (DataSize > MaxDataSize)
			return (int)Result::WrongSizing;

		//setup type
		((_PacketMarkup*)packet)->Type = (uint16_t) PacketTypes::Login;
		((_PacketMarkup*)packet)->DataSize = DataSize;

		_LoginMarkup* LoginInfo = (_LoginMarkup*)(((_PacketMarkup*)packet)->Data);
		//TODO: check string size before
		LoginInfo->LoginSize = GuessLogin.size()+1;
		LoginInfo->PassSize = GuessPass.size()+1;
		//copy this
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

