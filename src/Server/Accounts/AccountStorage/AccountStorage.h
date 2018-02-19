#pragma once
#include <string>
#include <vector>

#include <GlobalInfo.h>
#include <Utility\InPlace.h>
#include <Database\Database.h>


class AccountStorage : InPlace 
{

public:
    AccountStorage();
    bool CreatePrepared();


    ~AccountStorage();
    //adding new user 
    void NewUser(const std::string& login, const std::string& password);
    //deletin user
    bool DeleteUser(const std::string& login);
    //check, exist or not user with such login
    bool IsExist(const std::string& login);
    //TODO: remove
    size_t FetchUser(const std::string& login, const std::string& password);
    uint32_t GetID(const std::string& login);

    std::string GetLogin(ID_t ID);
    void RecordLogin(ID_t ID);
    void RecordLogout(ID_t ID);
    bool Online(ID_t ID); 

private:
    //TODO: we dont need string actually, is just to easy create query string
    static const std::string _UsersTable;
    
    //Prepared
    sql::PreparedStatement* _Prepared_NewUser;
    sql::PreparedStatement* _Prepared_DeleteUser;
    sql::PreparedStatement* _Prepared_IsExist;
    sql::PreparedStatement* _Prepared_IsCorrect;
    sql::PreparedStatement* _Prepared_GetId;
    sql::PreparedStatement* _Prepared_GetLogin;
};
