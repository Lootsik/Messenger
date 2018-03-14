#pragma once
#include <string>
#include <vector>

#pragma warning(push)
#pragma warning( disable : 4251 )
#include <cppconn\exception.h>
#pragma warning( pop )

#include <Protocol\GlobalInfo.h>

#include <Utility\InPlace.h>

#include <Database\Database.h>


// if arguments not incorrect false returned
// if error with databasee occur, sql::SQLException will throw
class AccountStorage : InPlace 
{
public:
    AccountStorage();
	~AccountStorage();


    //adding new user 
    bool NewUser(const std::string& login, const std::string& password_hash);
    

	// TODO: add some info to return value
	// rework through id
	// not user for now 
    bool DeleteUser(const std::string& login);



    //check, exist or not user with such login
	// not user for now 
	bool IsExist(const std::string& login);
    
	

	// check, that is login corresponded to pass,
	// if yes, returns id of such user, if not - INVALID_ ID
	size_t VerifyUser(const std::string& login, const std::string& password_hash);

	
	
	// returns ID of user with this login,
	// if no such user, INVALID_ID returned
	uint32_t GetID(const std::string& login);



	// returns login corresponding to ID,
	// if no such user, empty string returned
    std::string GetLogin(ID_t ID);


	// Set online
    void RecordLogin(ID_t ID);
	
	
	// set offline
    void RecordLogout(ID_t ID);
	
	
	// check, online such user or not
    bool Online(ID_t ID); 


private:


	// initializating of prepared statement
    bool CreatePrepared();


    // ease to create query string
    static const std::string _UsersTable;
    
    // prepared
    sql::PreparedStatement* _Prepared_NewUser;
    sql::PreparedStatement* _Prepared_DeleteUser;
    sql::PreparedStatement* _Prepared_IsExist;
    sql::PreparedStatement* _Prepared_IsCorrect;
    sql::PreparedStatement* _Prepared_GetId;
    sql::PreparedStatement* _Prepared_GetLogin;
};
