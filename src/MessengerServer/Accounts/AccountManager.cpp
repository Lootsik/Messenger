#include <Accounts\AccountStorage\AccountStorage.h>
#include <Server\Connection.h>
#include <Accounts\AuthenticationData.h>
#include "AccountManager.h"

//CRITICAL: login and logout is non 'atomic' operation in term of system, so
//we need that chek pass and login will be one operation, before add new threads or servers

AccountManager::AccountManager()
{
	if ((!_AccountStorage.Connect("tcp://127.0.0.1:3306", "root", "11JustLikeYou11") ||
			!_AccountStorage.CreatePrepared("messeger_server_db", "users")))
		throw;
}

ID_t AccountManager::VerifyAccount(const AuthenticationData& AuthData)
{
	size_t id = _AccountStorage.FetchUser(AuthData.GetLogin(), AuthData.GetPassword());
	//TODO: smth with this
	/*
	if (id > UINT32_MAX)
		throw;
	*/
	return static_cast<uint32_t>(id);
}

AuthenticationResult AccountManager::Login( const AuthenticationData& AuthData )
{
	//TODO: AccountStorage action
	ID_t id = VerifyAccount(AuthData);
	//wrong pass or login
	if (id == INVALID_ID)
		return AuthenticationResult{ (int)Packet::LoginResult::Result::Wrong };

	//check online
	if (_AccountStorage.Online(id))
		return AuthenticationResult{ (int)Packet::LoginResult::Result::AccInUse };

	//Actually change database
	_AccountStorage.RecordLogin(id);
	
	return AuthenticationResult{ (int)Packet::LoginResult::Result::Success, id };
}
//no check, pure database change
void AccountManager::Logout( ID_t& ID)
{
	_AccountStorage.RecordLogout(ID);
	ID = INVALID_ID;
}

bool AccountManager::UserOnline(ID_t ID)
{
	return _AccountStorage.Online(ID);
}