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

const std::string AccountStorage::_UsersTable = "users";


AccountStorage::AccountStorage()
{
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
        Logger::LogBoth(Logger::Error, "SQLException thrown on creating prepared. %s:%s. Error # %d: %s",
                                             __FILE__, __func__ ,e.getErrorCode(), e.what());
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
        //Amount of updated rows, 0 or 1 becouse login is unical
        size_t update = _Prepared_DeleteUser->executeUpdate();
        return update > 0;
    }
    //TODO: log + 
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
    //
    return id;
}
/* dont need now
std::vector<std::pair<unsigned int , std::string> > AccountStorage::FillLogins()
{
    std::vector<std::pair<unsigned int,std::string> > Output;

    boost::scoped_ptr<sql::ResultSet> _Result;

    try
    {
        _Result.reset(_DB.GetStatement()->executeQuery("SELECT id, Login FROM " + _UsersTable + ";"));
        while (_Result->next()) {
            Output.push_back(std::pair<unsigned int, std::string>{ _Result->getInt("id"),_Result->getString("Login") });
        }
    }
    catch (...)
    {
        throw;
    }
    return Output;
    }*/

ID_t AccountStorage::GetID(const std::string& login)
{
    boost::scoped_ptr<sql::ResultSet> _Result;
    try
    {
        _Prepared_GetId->clearParameters();
        _Prepared_GetId->setString(1, login);
        
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
        _Result.reset(Database::GetStatement().executeQuery("SELECT Login FROM " + _UsersTable + " WHERE id=" + std::to_string(ID) + ';'));
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
    catch (...)
    {
        throw;
    }
    return false;
}
