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
#include "AccountStorage.h"


AccountStorage::AccountStorage()
{
}
//TODO: check error codes
bool AccountStorage::Connect(const std::string& hostname, const std::string& login, const std::string& password)
{
	try
	{
		_Driver = get_driver_instance();
		_Connection = _Driver->connect(hostname, login, password);
	}
	catch (sql::SQLException& e)
	{
		Logger::LogBoth(Logger::Error, "SQLException thrown on connection to db. Error # %d: %s", e.getErrorCode(), e.what());
		return false;
	}
	catch (std::exception &e) 
	{
		Logger::LogBoth(Logger::Error, "Exeption thrown on connection to db: %s", e.what());
		return false;
	}
	return true;
}

bool AccountStorage::CreatePrepared(const std::string& schema, const std::string& table)
{
	_Table = table;

	try
	{
		_Connection->setSchema(schema);
		
		_Statement = _Connection->createStatement();

		_Prepared_NewUser = _Connection->prepareStatement("CALL NewUser(?,?);");
		_Prepared_DeleteUser = _Connection->prepareStatement("CALL DeleteUser(?);");
		_Prepared_IsExist = _Connection->prepareStatement("SELECT id FROM " + _Table + " WHERE Login = ?;");
		_Prepared_IsCorrect = _Connection->prepareStatement("SELECT id FROM " + _Table + " WHERE Login = ? AND Password = ?;");
		_Prepared_GetId = _Connection->prepareStatement("SELECT GetId(?);");
		_Prepared_GetLogin = _Connection->prepareStatement("SELECT GetLogin(?);");
	}
	catch (sql::SQLException& e)
	{
		Logger::LogBoth(Logger::Error, "SQLException thrown on creating prepared. Error # %d: %s", __func__,e.getErrorCode(), e.what());
		return false;
	}
	catch (std::exception &e)
	{
		Logger::LogBoth(Logger::Error, "Exeption thrown on creating prepared: %s", e.what());
		return false;
	}
	return true;
}

AccountStorage::~AccountStorage()
{
	delete _Prepared_NewUser;
	delete  _Prepared_DeleteUser;
	delete  _Prepared_IsExist;
	delete  _Prepared_IsCorrect;

	delete _Statement;
	delete _Connection;
}
	
void AccountStorage::NewUser(const std::string& login, const std::string& password)
{
	try
	{
		_Prepared_NewUser->clearParameters();
		_Prepared_NewUser->setString(1, login);
		_Prepared_NewUser->setString(2, password);
		
		//при любой ошибке в этом запросе выбросится исключение
		_Prepared_NewUser->execute();
	}
	//TODO: обработать
	catch (sql::SQLException &) {
		/*cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;*/
		throw;
	}
}

bool AccountStorage::DeleteUser(const std::string& login)
{
	try
	{
		_Prepared_DeleteUser->clearParameters();
		_Prepared_DeleteUser->setString(1, login);
		//тут количество удалённых записей (тут 1 или 0), потому что Login уникальный
		size_t update = _Prepared_DeleteUser->executeUpdate();
		return update > 0;
	}
	//TODO: обработать
	catch (sql::SQLException &) {
		throw;
	}

	return false;
}

bool AccountStorage::IsExist(const std::string& login)
{
	boost::scoped_ptr<sql::ResultSet> _Result;

	try
	{
		_Prepared_IsExist->clearParameters();
		_Prepared_IsExist->setString(1, login);
		//выполяем
		_Result.reset(  _Prepared_IsExist->executeQuery());
	}
	//TODO: обработать
	catch (...)
	{
		throw;
	}
	//количество строк в выходной таблице
	size_t rows = _Result->rowsCount();
	//rows всегда будет 1 или 2, тк поле Login уникальное
	return rows > 0;
}
	
//TODO: заменить функцией sql, заменить название
size_t AccountStorage::FetchUser(const std::string& login, const std::string& password)
{
	boost::scoped_ptr<sql::ResultSet> _Result;

	try
	{
		_Prepared_IsCorrect->clearParameters();
		_Prepared_IsCorrect->setString(1, login);
		_Prepared_IsCorrect->setString(2, password);

		_Result.reset( _Prepared_IsCorrect->executeQuery());
	}
	//TODO: обработать
	catch (...)
	{
		throw;
	}
	size_t rows = _Result->rowsCount();
	if (rows == 0)
	{
		return 0;
	}
	_Result->next();
	uint32_t id = _Result->getInt("id");
	//если 
	return id;
}
//TODO: обработать ошибки тут и по всему модулю
std::vector<std::pair<unsigned int , std::string> > AccountStorage::FillLogins()
{
	std::vector<std::pair<unsigned int,std::string> > Output;

	boost::scoped_ptr<sql::ResultSet> _Result;

	try
	{
		_Result.reset(_Statement->executeQuery("SELECT id, Login FROM " + _Table + ";"));
		while (_Result->next()) {
			Output.push_back(std::pair<unsigned int, std::string>{ _Result->getInt("id"),_Result->getString("Login") });
		}
	}
	catch (...)
	{
		throw;
	}
	return Output;
}

ID_t AccountStorage::GetID(const std::string& login)
{
	boost::scoped_ptr<sql::ResultSet> _Result;
	try
	{
		_Prepared_GetId->clearParameters();
		_Prepared_GetId->setString(1, login);
		//выполяем
		_Result.reset( _Prepared_GetId->executeQuery());
	}
	catch (...)
	{
		throw;
	}
	if (_Result->next())
		return  _Result->getInt("id");
	else
		return INVALID_ID;
}
std::string AccountStorage::GetLogin(ID_t ID)
{
	boost::scoped_ptr<sql::ResultSet> _Result;

	try
	{
		_Result.reset(_Statement->executeQuery("SELECT Login FROM " + _Table + " WHERE id=" + std::to_string(ID) + ';'));
	}
	catch (...)
	{
		throw;
	}
	if (_Result->next())
		return _Result->getString("Login");
		
	return "";
}
void AccountStorage::RecordLogin(ID_t ID)
{
	_Statement->executeUpdate("UPDATE " + _Table + " SET Online=1 WHERE id = " + std::to_string(ID) + ';');
}
void AccountStorage::RecordLogout(ID_t ID)
{
	_Statement->executeUpdate("UPDATE " + _Table + " SET Online=0 WHERE id = " + std::to_string(ID) + ';');
}
bool AccountStorage::Online(ID_t ID)
{
	boost::scoped_ptr<sql::ResultSet> _Result;
	try
	{
		_Result.reset(_Statement->executeQuery("SELECT Online FROM " + _Table + " WHERE id = " + std::to_string(ID) + ';'));
		size_t count  = _Result->rowsCount();
		while (_Result->next()) {
			return static_cast<bool>(_Result->getInt("Online"));
		}
	}
	catch (...)
	{
		throw;
	}
	return false;
}