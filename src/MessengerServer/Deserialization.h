#pragma once
#include "PacketFormat.h"
#include <string>



namespace Deserialization 
{
	enum class Result
	{
		Ok,
		WrongSizing
	};
	//basic chek for now: if size in packet equal to size, that recived, that ok
	bool PacketCheckup(const char* paket, size_t size);
	int PaketType(const char* paket, size_t size);

	int OnLogout();
	int OnLogin(const char* paket, size_t size, std::string& LoginFrom, std::string& PassFrom );

}