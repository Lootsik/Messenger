#pragma once
#include <string>
#include <GlobalInfo.h>
#include <PacketFormat/PacketFormat.h>
#include <Accounts/AccountStorage/AccountStorage.h>
#include <Accounts/AuthenticationResult.h>
class AuthenticationData;

struct AccountManager
{
	AccountManager();

	ID_t VerifyAccount(const AuthenticationData& AuthData);
	AuthenticationResult Login(const AuthenticationData& AuthData);
	void Logout(ID_t& ID);
	bool UserOnline(ID_t ID);

private:
	AccountStorage _AccountStorage;
};