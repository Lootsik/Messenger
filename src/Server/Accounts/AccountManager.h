#pragma once
#include <string>
#include <Protocol\GlobalInfo.h>

#include <Accounts\AccountStorage.h>

#include <Protocol\Types\LoginRequest.h>
#include <Protocol\Types\LoginResponse.h>

class AccountManager
{
public:
	AccountManager();


	LoginResponse Login(const LoginRequest& AuthData);


	
	// returs login corresponding to this ID
	std::string FindLogin(ID_t ID);



	void Logout(ID_t ID);
	
	
	// user with ID online now
	bool UserOnline(ID_t ID);

	
	bool AddUser(const std::string& Login, const std::string& Password);

	// maximum sizes of login and pass
	static const unsigned LoginMax = Storing::LoginSizeMax;
	static const unsigned PasswordMax = Storing::PasswordSizeMax;

private:
	// checks, that login corresponded to password, 
	// if yes returns id of such user
	ID_t VerifyAccount(const LoginRequest& AuthData);
	
	AccountStorage _AccountStorage;
};