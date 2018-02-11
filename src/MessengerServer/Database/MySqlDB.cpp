#pragma warning(push)
#pragma warning( disable : 4251 )
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#pragma warning(pop)

#include <Logger/Logger.h>
#include "MySqlDB.h"



MySqlDB::MySqlDB()
{
}

bool MySqlDB::Connect(const std::string& hostname, const std::string schema, 
							 const std::string& login, const std::string& password)
{
	try
	{
		_Driver = get_driver_instance();
		_Connection = _Driver->connect(hostname, login, password);
		_Connection->setSchema(schema);
	}
	catch (sql::SQLException& e)
	{
		Logger::LogBoth(Logger::Error, "SQLException thrown on connection to db. Error # %d: %s", e.getErrorCode(), e.what());
		return false;
	}
	return true;
}

sql::Connection& MySqlDB::GetConnection()
{
	return *_Connection;
}
sql::Statement& MySqlDB::GetStatement()
{
	return *_Statement;
}

//free data
void MySqlDB::_Cleanup()
{
	delete _Statement;
	delete _Connection;
}

void MySqlDB::CloseConnection()
{
	try
	{
		if( _Connection )
			_Connection->close();
		//if connection assigned, _Driver assigned too
		_Driver->threadEnd();
	}
	catch (sql::SQLException &e)
	{
		//TODO: write Log and make strategy to handle 
		throw;
	}
}

bool MySqlDB::Connected()
{
	return !_Connection->isClosed();
}

MySqlDB::~MySqlDB()
{
	CloseConnection();
	_Cleanup();
}
	