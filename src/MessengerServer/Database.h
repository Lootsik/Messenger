#pragma once
#include <string>
#include <vector>
//TODO: заменить mysql

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
//TODO: пофиксить размеры в полей, не сделать случайно case insesetive, сделать что-то с кодировками

//пересоздать дб
class Database {
public:
	Database(const std::string& hostname, const std::string& login, const std::string& password,
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
	bool IsCorrect(const std::string& login, const std::string& password);

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
