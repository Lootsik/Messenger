#define _CRT_SECURE_NO_WARNINGS
#include <Protocol\GlobalInfo.h>

#include "UserInfoRules.h"

const char AllowedSymbols[] = { '@','!', '_','-','(',')' };

static bool IsAllowedString(const std::string& str)
{
	for (unsigned char symbol : str)
	{
		//checks that symbol is not digit, not letter and not allowed symbol
		if (!isalpha(symbol) && !isdigit(symbol) &&
			std::find(AllowedSymbols, AllowedSymbols + sizeof(AllowedSymbols), symbol) ==
			AllowedSymbols + sizeof(AllowedSymbols))
			return false;
	}
	return true;
}

bool IsAllowedLogin(const std::string& Login)
{
	if (Login.size() > Storing::LoginSizeMax)
		return false;

	return IsAllowedString(Login);
}

bool IsAllowedPassword(const std::string& Password)
{
	if (Password.size() > Storing::PasswordSizeMax)
		return false;

	return IsAllowedString(Password);
}

std::string PasswordHash(const std::string& Password)
{
	unsigned char HashBuff[SHA256_DIGEST_LENGTH] = {};
	SHA256((unsigned char*)Password.c_str(), Password.size(), HashBuff);

	std::string HashString;
	HashString.resize(Storing::PasswordHashSize);

	char* Str = const_cast<char*>(HashString.c_str());

	for (size_t i = 0; i < SHA256_DIGEST_LENGTH; ++i)
		sprintf(&(Str[i * 2]), "%02x", HashBuff[i]);

	return HashString;
}