#include <Accounts\AccountStorage\AccountStorage.h>

#include <Network\Connection.h>

#include <Protocol\LoginRequest.h>
#include <Protocol\LoginResponse.h>

#include "AccountManager.h"

//CRITICAL: login and logout is non 'atomic' operation in term of system, so
//we need that chek pass and login will be one operation, before add new threads or servers

AccountManager::AccountManager(){}

ID_t AccountManager::VerifyAccount(const LoginRequest& AuthData)
{
	size_t id = _AccountStorage.VerifyUser(AuthData.GetLogin(), AuthData.GetPassword());
	
	return static_cast<uint32_t>(id);
}

LoginResponse AccountManager::Login( const LoginRequest& AuthData )
{
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