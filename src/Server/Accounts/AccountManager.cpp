#include <Protocol\UserInfoRules.h>

#include "AccountManager.h"

//CRITICAL: login and logout is non 'atomic' operation in term of system, so
//we need that chek pass and login will be one operation, before add new threads or servers

AccountManager::AccountManager(){}

ID_t AccountManager::VerifyAccount(const LoginRequest& AuthData)
{
	if (BytesContain(AuthData.GetLogin()) > LoginMax ||
			BytesContain(AuthData.GetPasswordHash()) > Storing::PasswordHashSize)
		return INVALID_ID;

	size_t id = _AccountStorage.VerifyUser(AuthData.GetLogin(), AuthData.GetPasswordHash());
	
	return static_cast<uint32_t>(id);
}



LoginResponse AccountManager::Login( const LoginRequest& AuthData )
{
	if (BytesContain(AuthData.GetLogin()) > LoginMax ||
			BytesContain(AuthData.GetPasswordHash()) != Storing::PasswordHashSize)
		return { LoginResponse::Wrong };

	ID_t id = VerifyAccount(AuthData);
	//wrong pass or login
	if (id == INVALID_ID)
		return LoginResponse{ LoginResponse::Wrong };

	//check online
	if (_AccountStorage.Online(id))
		return LoginResponse{ LoginResponse::AccInUse };

	//Actually change database
	_AccountStorage.RecordLogin(id);
	
	return LoginResponse{ LoginResponse::Success, id };
}

// TODO: write
bool AccountManager::AddUser(const std::string& Login, const std::string& Password)
{
	return IsAllowedLogin(Login);
}




//no check, pure database change
void AccountManager::Logout( ID_t ID)
{
	_AccountStorage.RecordLogout(ID);
}



bool AccountManager::UserOnline(ID_t ID)
{
	return _AccountStorage.Online(ID);
}



std::string AccountManager::FindLogin(ID_t ID)
{
	return _AccountStorage.GetLogin(ID);
}