#pragma once
#include "PacketFormat.h"
#include <string>



namespace Deserialization
{
	enum Result
	{
		Ok,
		WrongSizing
	};
	//basic chek for now: if size in packet equal to size, that recived, that ok
	bool PacketCheckup(const BYTE* packet, size_t size);
	int PaketType(const BYTE* packet, size_t size);

	int OnLogout(const BYTE* packet, size_t size);
	int OnLogin(const BYTE* packet, size_t size, std::string& LoginFrom, std::string& PassFrom );
	//изменить аргументы
	int OnMessage(const BYTE* packet, size_t size, uint32_t& from, uint32_t& to,uint32_t& index, uint32_t& MessSize, BYTE** Message);
	int OnLoginResult(const BYTE* packet, uint32_t& result, uint32_t& id);
}