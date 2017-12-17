#include "Deserialization.h"

namespace Deserialization 
{
	bool PacketCheckup(const char* paket, size_t size)
	{
		return ((_PacketMarkup*)paket)->Size == size;
	}

	int PaketType(const char* paket, size_t size)
	{
		return ((_PacketMarkup*)paket)->Type;
	}			

	int OnLogout()
	{
		return 0;
	}
	int OnLogin(const char* paket, size_t size, std::string& LoginFrom, std::string& PassFrom)
	{
		//TODO: hide this 4
		if (size < 4)
			return (int)Result::WrongSizing;

		const char* Mes = paket;

		unsigned long SizeLogin = *(unsigned long*)Mes;
		Mes = (char*)((unsigned long*)Mes + 1);

		unsigned long SizePass = *(unsigned long*)Mes;
		Mes = (char*)((unsigned long*)Mes + 1);

		if (4 + SizePass + SizeLogin > size)
			return (int)Result::WrongSizing;

		//CHECK: end can be not in
		LoginFrom = std::string{ Mes, Mes + SizeLogin };
		PassFrom = std::string{ Mes + SizeLogin, Mes + SizeLogin + SizePass };

		return (int)Result::Ok;
	}

}