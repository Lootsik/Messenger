#pragma once
#include <string>
#include <vector>

namespace sql {
	class Driver;
	class Connection;
	class Statement;
	class ResultSet;
	class PreparedStatement;
}

//TODO: ��������� ������� � �����, �� ������� �������� case insesetive, ������� ���-�� � �����������

//����������� ��
class Database {
public:
	Database();
	bool Connect(const std::string& hostname, const std::string& login, const std::string& password,
									const std::string& schema, const std::string& table);
	
	//TODO: �������� set get ������ � ��������� ��� 
	//��������� ����������� � �����������, ��� ����� �������, �� ��� ���������� 
	Database (const Database&) = delete;
	Database& operator=(const Database&) = delete;
	Database (Database&&) = delete;
	Database& operator=(Database &&) = delete;

	~Database();
	//���������� ������ ������������
	void NewUser(const std::string& login, const std::string& password);
	//�������� ������������
	bool DeleteUser(const std::string& login);
	//�������� �� ������������� ������������ � ����� �������
	bool IsExist(const std::string& login);
	//TODO: �������� �������� sql
	size_t FetchUser(const std::string& login, const std::string& password);

	std::vector<std::pair<unsigned int, std::string>> FillLogins();

private:
	//TODO: �������� ��������� ������� ��������� ����� ��
	std::string _Table;

	sql::Driver* _Driver;
	sql::Connection* _Connection;
	sql::Statement* _Statement;
	sql::ResultSet* _Result;
	//Prepared
	sql::PreparedStatement* _Prepared_NewUser;
	sql::PreparedStatement* _Prepared_DeleteUser;
	sql::PreparedStatement* _Prepared_IsExist;
	sql::PreparedStatement* _Prepared_IsCorrect;
};
