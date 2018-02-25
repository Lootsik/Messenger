#include <Database\DbInternals.h>

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
		_Statement = _Connection->createStatement();
	}
	catch (sql::SQLException& e)
	{
		Logger::LogBoth(Logger::Error, "SQL error on connection to db. Error # %d: %s", e.getErrorCode(), e.what());
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
		FallLog();
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
	