#include <Database\DbInternals.h>
#include <Logger\Logger.h>

#include "AccountStorage.h"

const std::string AccountStorage::_UsersTable = "users";


AccountStorage::AccountStorage()
{
	CreatePrepared();
}

bool AccountStorage::CreatePrepared()
{
    sql::Connection& Conn = Database::GetConnection();
    try
    {
        _Prepared_NewUser = Conn.prepareStatement("CALL NewUser(?,?);");
        _Prepared_DeleteUser = Conn.prepareStatement("CALL DeleteUser(?);");
        _Prepared_IsExist = Conn.prepareStatement("SELECT id FROM " + std::string{ _UsersTable }+" WHERE Login = ?;");
        _Prepared_IsCorrect = Conn.prepareStatement("SELECT id FROM " + _UsersTable + " WHERE Login = ? AND Password = ?;");
        _Prepared_GetId = Conn.prepareStatement("SELECT GetId(?);");
        _Prepared_GetLogin = Conn.prepareStatement("SELECT GetLogin(?);");
    }
    catch (sql::SQLException& e)
    {
		FallLog();
    }
    return true;
}

AccountStorage::~AccountStorage()
{
    delete _Prepared_NewUser;
    delete _Prepared_DeleteUser;
    delete _Prepared_IsExist;
    delete _Prepared_IsCorrect;
    delete _Prepared_GetId;
    delete _Prepared_GetLogin;
}
    
void AccountStorage::NewUser(const std::string& login, const std::string& password)
{
    try
    {
        _Prepared_NewUser->clearParameters();
        _Prepared_NewUser->setString(1, login);
        _Prepared_NewUser->setString(2, password);
        
        _Prepared_NewUser->execute();
    }
    //TODO: log + 
    catch (sql::SQLException &e) {
		FallLog();
        throw;
    }
}

bool AccountStorage::DeleteUser(const std::string& login)
{
    try
    {
        _Prepared_DeleteUser->clearParameters();
        _Prepared_DeleteUser->setString(1, login);
        //Amount of updated rows, 0 or 1 becouse login is unical
        size_t update = _Prepared_DeleteUser->executeUpdate();
        return update > 0;
    }
    //TODO: log + 
    catch (sql::SQLException &e) {
		FallLog();
    }

}

bool AccountStorage::IsExist(const std::string& login)
{
    boost::scoped_ptr<sql::ResultSet> _Result;

    try
    {
        _Prepared_IsExist->clearParameters();
        _Prepared_IsExist->setString(1, login);
        
        _Result.reset(  _Prepared_IsExist->executeQuery());
    }
    //TODO: Log + 
    catch (...)
    {
        throw;
    }
    //number of rows in output table
    size_t rows = _Result->rowsCount();
    //awlays 0 or 1 becouse login identical
    return rows > 0;
}
    

//TODO: change name + checks 
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
    //TODO: log + 
    catch (sql::SQLException &e)
    {
		FallLog();
    }
    size_t rows = _Result->rowsCount();
    if (rows == 0)
    {
        return 0;
    }
	_Result->next();
    uint32_t id = _Result->getInt("id");

    return id;
}

ID_t AccountStorage::GetID(const std::string& login)
{
    boost::scoped_ptr<sql::ResultSet> _Result;
    try
    {
        _Prepared_GetId->clearParameters();
        _Prepared_GetId->setString(1, login);
        
        _Result.reset( _Prepared_GetId->executeQuery());
    }
    catch (sql::SQLException &e)
    {
		FallLog();
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
        _Result.reset(Database::GetStatement().executeQuery("SELECT Login FROM " + _UsersTable + " WHERE id=" + std::to_string(ID) + ';'));
    }
    catch (sql::SQLException &e)
    {
		FallLog();
    }
    if (_Result->next())
        return _Result->getString("Login");
        
    return "";
}
void AccountStorage::RecordLogin(ID_t ID)
{
    Database::GetStatement().executeUpdate("UPDATE " + _UsersTable + " SET Online=1 WHERE id = " + std::to_string(ID) + ';');
}
void AccountStorage::RecordLogout(ID_t ID)
{
	Database::GetStatement().executeUpdate("UPDATE " + _UsersTable + " SET Online=0 WHERE id = " + std::to_string(ID) + ';');
}
bool AccountStorage::Online(ID_t ID)
{
    boost::scoped_ptr<sql::ResultSet> _Result;
    try
    {
        _Result.reset(Database::GetStatement().executeQuery("SELECT Online FROM " + _UsersTable + " WHERE id = " + std::to_string(ID) + ';'));
        size_t count  = _Result->rowsCount();
        while (_Result->next()) {
            return static_cast<bool>(_Result->getInt("Online"));
        }
    }
    catch (sql::SQLException &e)
    {
		FallLog();
    }
    return false;
}
