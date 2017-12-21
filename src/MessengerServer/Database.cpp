#include "Database.h"

#include <string>



Database::Database(const std::string& hostname, const std::string& login, const std::string& password,
				   const std::string& schema, const std::string& table)
	:_Table{ table }
{
	try
	{
		_Driver = get_driver_instance();
		//обработка ошибок 
		try
		{
			//коннектимся
			_Connection = _Driver->connect(hostname, login, password);
		}
		catch (const std::bad_alloc&)
		{
			//не выделилась память
			throw;
		}
		catch (...)
		{
			//другие ошибки, но память уже выделилась
			delete _Connection;
			throw;
		}

		//меняем схему
		_Connection->setSchema(schema);

		try
		{
			//создаём выражение
			_Statement = _Connection->createStatement();
		}
		catch (const std::bad_alloc&)
		{
			//ошибка выделения памяти, освобождаем Connection так как на него уже выделена память
			delete _Connection;
			throw;
		}
		catch (...)
		{
			//любые другие ошибки 
			delete _Statement;
			delete _Connection;
			throw;
		}

		//TODO: тут тоже могут вылезти исключения, нужно обработать
		_Prepared_NewUser = _Connection->prepareStatement("CALL NewUser(?,?);");
		_Prepared_DeleteUser = _Connection->prepareStatement("CALL DeleteUser(?);");
		_Prepared_IsExist = _Connection->prepareStatement("SELECT id FROM " + _Table + " WHERE Login = ?;");
		_Prepared_IsCorrect = _Connection->prepareStatement("SELECT id FROM " + _Table + " WHERE Login = ? AND Password = ?;");
	}
	catch (...)
	{
		throw;
	}
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
		
		//при любой ошибке в этом запросе выбросится исключение
		_Prepared_NewUser->execute();
	}
	//TODO: обработать
	catch (sql::SQLException &e) {
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

bool Database::IsExist(const std::string& login)
{
	try
	{
		_Prepared_IsExist->clearParameters();
		_Prepared_IsExist->setString(1, login);
		//выполяем
		_Result = _Prepared_IsExist->executeQuery();
	}
	//TODO: обработать
	catch (...)
	{
		throw;
	}
	//количество строк в выходной таблице
	size_t rows = _Result->rowsCount();
	delete _Result;
	//rows всегда будет 1 или 2, тк поле Login уникальное
	return rows > 0;
}
	
//TODO: заменить функцией sql, заменить название
bool Database::IsCorrect(const std::string& login, const std::string& password)
{
	try
	{
		_Prepared_IsCorrect->clearParameters();
		_Prepared_IsCorrect->setString(1, login);
		_Prepared_IsCorrect->setString(2, password);
		//открыто для иньекции, сделать что-то
		_Result = _Prepared_IsCorrect->executeQuery();
	}
	//TODO: обработать
	catch (...)
	{
		throw;
	}
	size_t RowCount = _Result->rowsCount();
	delete _Result;
	//если 
	return RowCount > 0;
}
//TODO: обработать ошибки тут и по всему модулю
std::vector<std::pair<unsigned int , std::string> > Database::FillLogins()
{
	std::vector<std::pair<unsigned int,std::string> > Output;

	try
	{
		_Result = _Statement->executeQuery("SELECT * FROM " + _Table + ";");
		while (_Result->next()) {
			Output.push_back(std::pair<unsigned int, std::string>{ _Result->getInt("Id"),_Result->getString("Login") });
		}
	}
	catch (...)
	{
		throw;
	}
	return Output;
}