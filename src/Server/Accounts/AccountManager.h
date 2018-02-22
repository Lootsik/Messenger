#pragma once
#include <string>
#include <Protocol\GlobalInfo.h>

#include <Accounts\AccountStorage\AccountStorage.h>

#include <Protocol\LoginRequest.h>
#include <Protocol\LoginResponse.h>

class AccountManager
{
public:
	AccountManager();

	ID_t VerifyAccount(const LoginRequest& AuthData);
	LoginResponse Login(const LoginRequest& AuthData);

	std::string FindLogin(ID_t ID);

	void Logout(ID_t ID);
	bool UserOnline(ID_t ID);

private:
	AccountStorage _AccountStorage;
};