#pragma warning(push)
#pragma warning( disable : 4251 )
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#pragma warning(pop)

#include "Database.h"

Database::Database()
{}
//TODO: check error codes
bool Database::Connect(const std::string& hostname, const std::string& login, const std::string& password,
				   const std::string& schema, const std::string& table)
{
	_Table = table;
	try
	{
		_Driver = get_driver_instance();
	
		try
		{
			//connecting
			_Connection = _Driver->connect(hostname, login, password);
		}
		catch (const std::bad_alloc&)
		{
			throw;
		}
		catch (...)
		{
			//other errors but memory already allocated
			delete _Connection;
			throw;
		}
		//change schema
		_Connection->setSchema(schema);

		try
		{
			_Statement = _Connection->createStatement();
		}
		catch (const std::bad_alloc&)
		{
			//������ ��������� ������, ����������� Connection ��� ��� �� ���� ��� �������� ������
			delete _Connection;
			throw;
		}
		catch (...)
		{
			//����� ������ ������ 
			delete _Statement;
			delete _Connection;
			throw;
		}

		//TODO: ��� ���� ����� ������� ����������, ����� ����������
		_Prepared_NewUser = _Connection->prepareStatement("CALL NewUser(?,?);");
		_Prepared_DeleteUser = _Connection->prepareStatement("CALL DeleteUser(?);");
		_Prepared_IsExist = _Connection->prepareStatement("SELECT id FROM " + _Table + " WHERE Login = ?;");
		_Prepared_IsCorrect = _Connection->prepareStatement("SELECT id FROM " + _Table + " WHERE Login = ? AND Password = ?;");
	}
	catch (...)
	{
		//anyway, if we get error here, we will program
		//so its not necessary release all resources
		return false;
	}
	return true;
}

Database::~Database()
{
	delete _Prepared_NewUser;
	delete  _Prepared_DeleteUser;
	delete  _Prepared_IsExist;
	delete  _Prepared_IsCorrect;

	delete _Statement;
	delete _Connection;
}
	
void Database::NewUser(const std::string& login, const std::string& password)
{
	try
	{
		_Prepared_NewUser->clearParameters();
		_Prepared_NewUser->setString(1, login);
		_Prepared_NewUser->setString(2, password);
		
		//��� ����� ������ � ���� ������� ���������� ����������
		_Prepared_NewUser->execute();
	}
	//TODO: ����������
	catch (sql::SQLException &) {
		/*cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;*/
		throw;
	}
}

bool Database::DeleteUser(const std::string& login)
{
	try
	{
		_Prepared_DeleteUser->clearParameters();
		_Prepared_DeleteUser->setString(1, login);
		//��� ���������� �������� ������� (��� 1 ��� 0), ������ ��� Login ����������
		size_t update = _Prepared_DeleteUser->executeUpdate();
		return update > 0;
	}
	//TODO: ����������
	catch (sql::SQLException &) {
		throw;
	}

	return false;
}

bool Database::IsExist(const std::string& login)
{
	try
	{
		_Prepared_IsExist->clearParameters();
		_Prepared_IsExist->setString(1, login);
		//��������
		_Result = _Prepared_IsExist->executeQuery();
	}
	//TODO: ����������
	catch (...)
	{
		throw;
	}
	//���������� ����� � �������� �������
	size_t rows = _Result->rowsCount();
	delete _Result;
	//rows ������ ����� 1 ��� 2, �� ���� Login ����������
	return rows > 0;
}
	
//TODO: �������� �������� sql, �������� ��������
size_t Database::FetchUser(const std::string& login, const std::string& password)
{
	try
	{
		_Prepared_IsCorrect->clearParameters();
		_Prepared_IsCorrect->setString(1, login);
		_Prepared_IsCorrect->setString(2, password);

		_Result = _Prepared_IsCorrect->executeQuery();
	}
	//TODO: ����������
	catch (...)
	{
		throw;
	}
	size_t rows = _Result->rowsCount();
	if (rows == 0)
		return 0;

	_Result->next();
	size_t id = _Result->getInt64("id");
	delete _Result;
	//���� 
	return id;
}
//TODO: ���������� ������ ��� � �� ����� ������
std::vector<std::pair<unsigned int , std::string> > Database::FillLogins()
{
	std::vector<std::pair<unsigned int,std::string> > Output;

	try
	{
		_Result = _Statement->executeQuery("SELECT id, Login FROM " + _Table + ";");
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
