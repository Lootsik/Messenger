#pragma once

#include <Utility\InPlace.h>

namespace sql {
	class Driver;
	class Connection;
	class Statement;
	class ResultSet;
	class PreparedStatement;
}
class MySqlDB;

//to keep MySlqDB single instantion
class Database : InPlace 
{
public:
	static bool Connect(const std::string& hostname, const std::string schema,
						const std::string& login, const std::string& password);
	static void CloseConnection();
	static sql::Connection& GetConnection();
	static sql::Statement& GetStatement();
	static bool Connected();

private:
	static MySqlDB& DB();
	Database();
	~Database();
};

