#include "LoginRules.h"

const char AllowedSymbols[] = { '@','!', '_','-','(',')' };


bool IsAllowedLogin(const std::string& Login)
{
	for (unsigned char symbol : Login)
	{
		//checks that symbol is not digit, not letter and not allowed symbol
		if (!isalpha(symbol) && !isdigit(symbol) &&
			std::find(AllowedSymbols, AllowedSymbols + sizeof(AllowedSymbols), symbol) ==
			AllowedSymbols + sizeof(AllowedSymbols))
			return false;
	}
	return true;
}