#pragma once
#include <string>
#include <vector>
#include <GlobalInfo.h>
#include <Database\Database.h>


//TODO: пофиксить размеры в полей, не сделать случайно case insesetive, сделать что-то с кодировками

//пересоздать дб
class AccountStorage {
public:
    AccountStorage();
    bool CreatePrepared();

    //do not allow copy or move  
    AccountStorage (const AccountStorage&) = delete;
    AccountStorage& operator=(const AccountStorage&) = delete;
    AccountStorage (AccountStorage&&) = delete;
    AccountStorage& operator=(AccountStorage &&) = delete;

    ~AccountStorage();
    //добавление нового пользователя
    void NewUser(const std::string& login, const std::string& password);
    //удаление пользователя
    bool DeleteUser(const std::string& login);
    //проверка на существование пользователя с таким логином
    bool IsExist(const std::string& login);
    //TODO: заменить функцией sql
    size_t FetchUser(const std::string& login, const std::string& password);
    uint32_t GetID(const std::string& login);

    std::string GetLogin(ID_t ID);
    void RecordLogin(ID_t ID);
    void RecordLogout(ID_t ID);
    bool Online(ID_t ID); 

private:
    //TODO: we dont need string actually, is just to easy create query string
    static const std::string _UsersTable;
    
    //TODO: заменить некоторые запросы функциями самой бд
    //Prepared
    sql::PreparedStatement* _Prepared_NewUser;
    sql::PreparedStatement* _Prepared_DeleteUser;
    sql::PreparedStatement* _Prepared_IsExist;
    sql::PreparedStatement* _Prepared_IsCorrect;
    sql::PreparedStatement* _Prepared_GetId;
    sql::PreparedStatement* _Prepared_GetLogin;
};
