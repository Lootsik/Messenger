#pragma once
#include <string>

#include <Protocol\GlobalInfo.h>

//to hide includes inside .cpp 
namespace sql {
	class Driver;
	class Connection;
	class Statement;
	class ResultSet;
	class PreparedStatement;
}

class MySqlDB {
public:
	MySqlDB();
	~MySqlDB();
	//do not allow copy or move
	MySqlDB (const MySqlDB&) = delete;
	MySqlDB& operator=(const MySqlDB&) = delete;
	MySqlDB (MySqlDB&&) = delete;
	MySqlDB& operator=(MySqlDB &&) = delete;

	bool Connect(const std::string& hostname, const std::string schema,
		const std::string& login, const std::string& password);

	void CloseConnection();
	bool Connected();
	
	sql::Connection& GetConnection();
	sql::Statement& GetStatement();

private:
	void _Cleanup();

	sql::Driver* _Driver{};
	sql::Connection* _Connection{};
	sql::Statement* _Statement{};
};
