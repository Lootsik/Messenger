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

//TODO: пофиксить размеры в полей, не сделать случайно case insesetive, сделать что-то с кодировками

//пересоздать дб
class Database {
public:
	Database();
	bool Connect(const std::string& hostname, const std::string& login, const std::string& password,
									const std::string& schema, const std::string& table);
	
	//TODO: добавить set get методы и почистить код 
	//запрещаем копирование и перемещение, это можно сделать, но нет надобности 
	Database (const Database&) = delete;
	Database& operator=(const Database&) = delete;
	Database (Database&&) = delete;
	Database& operator=(Database &&) = delete;

	~Database();
	//добавление нового пользователя
	void NewUser(const std::string& login, const std::string& password);
	//удаление пользователя
	bool DeleteUser(const std::string& login);
	//проверка на существование пользователя с таким логином
	bool IsExist(const std::string& login);
	//TODO: заменить функцией sql
	size_t FetchUser(const std::string& login, const std::string& password);

	std::vector<std::pair<unsigned int, std::string>> FillLogins();

private:
	//TODO: заменить некоторые запросы функциями самой бд
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
