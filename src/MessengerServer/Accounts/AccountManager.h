#pragma once
#include <string>
#include <GlobalInfo.h>

#include <Protocol\LoginRequest.h>
#include <Protocol\LoginResponse.h>
class AuthenticationData;

struct AccountManager
{
	AccountManager();

	ID_t VerifyAccount(const LoginRequest& AuthData);
	LoginResponse Login(const LoginRequest& AuthData);
	void Logout(ID_t& ID);
	bool UserOnline(ID_t ID);

private:
	AccountStorage _AccountStorage;
};