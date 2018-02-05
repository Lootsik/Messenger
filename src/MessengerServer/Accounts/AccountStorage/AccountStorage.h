#pragma once
#include <string>
#include <vector>
#include <GlobalInfo.h>
namespace sql {
	class Driver;
	class Connection;
	class Statement;
	class ResultSet;
	class PreparedStatement;
}

//TODO: ��������� ������� � �����, �� ������� �������� case insesetive, ������� ���-�� � �����������

//����������� ��
class AccountStorage {
public:
	AccountStorage();
	bool Connect(const std::string& hostname, const std::string& login, const std::string& password);
	bool CreatePrepared(const std::string& schema, const std::string& table);

	//TODO: �������� set get ������ � ��������� ��� 
	//��������� ����������� � �����������, ��� ����� �������, �� ��� ���������� 
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

	std::vector<std::pair<unsigned int, std::string>> FillLogins();

	std::string GetLogin(ID_t ID);
	void RecordLogin(ID_t ID);
	void RecordLogout(ID_t ID);
	bool Online(ID_t ID); 


private:
	//TODO: �������� ��������� ������� ��������� ����� ��
	std::string _Table;

	sql::Driver* _Driver;
	sql::Connection* _Connection;
	sql::Statement* _Statement;
	//Prepared
	sql::PreparedStatement* _Prepared_NewUser;
	sql::PreparedStatement* _Prepared_DeleteUser;
	sql::PreparedStatement* _Prepared_IsExist;
	sql::PreparedStatement* _Prepared_IsCorrect;
	sql::PreparedStatement* _Prepared_GetId;
	sql::PreparedStatement* _Prepared_GetLogin;
};
