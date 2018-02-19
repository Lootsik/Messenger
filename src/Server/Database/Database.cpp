#include <Database\MySqlDB.h>

#include "Database.h"


Database::Database()
{
}


Database::~Database()
{
}

MySqlDB& Database::DB()
{
	static MySqlDB _DB;
	return _DB;
}

bool Database::Connect(const std::string& hostname, const std::string schema,
								const std::string& login, const std::string& password)
{
	return DB().Connect(hostname, schema, login, password);
}
void Database::CloseConnection()
{
	DB().CloseConnection();
}
sql::Connection& Database::GetConnection()
{
	return DB().GetConnection();
}
sql::Statement& Database::GetStatement()
{
	return DB().GetStatement();
}
bool Database::Connected() {
	return DB().Connected();
}

