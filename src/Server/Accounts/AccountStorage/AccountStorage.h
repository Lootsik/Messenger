#pragma once
#include <string>
#include <vector>
#include <GlobalInfo.h>
#include <Database\Database.h>


//TODO: ��������� ������� � �����, �� ������� �������� case insesetive, ������� ���-�� � �����������

//����������� ��
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
    //���������� ������ ������������
    void NewUser(const std::string& login, const std::string& password);
    //�������� ������������
    bool DeleteUser(const std::string& login);
    //�������� �� ������������� ������������ � ����� �������
    bool IsExist(const std::string& login);
    //TODO: �������� �������� sql
    size_t FetchUser(const std::string& login, const std::string& password);
    uint32_t GetID(const std::string& login);

    std::string GetLogin(ID_t ID);
    void RecordLogin(ID_t ID);
    void RecordLogout(ID_t ID);
    bool Online(ID_t ID); 

private:
    //TODO: we dont need string actually, is just to easy create query string
    static const std::string _UsersTable;
    
    //TODO: �������� ��������� ������� ��������� ����� ��
    //Prepared
    sql::PreparedStatement* _Prepared_NewUser;
    sql::PreparedStatement* _Prepared_DeleteUser;
    sql::PreparedStatement* _Prepared_IsExist;
    sql::PreparedStatement* _Prepared_IsCorrect;
    sql::PreparedStatement* _Prepared_GetId;
    sql::PreparedStatement* _Prepared_GetLogin;
};
