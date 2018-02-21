#include <Database\DbInternals.h>

#include <Logger/Logger.h>

#include "MessagesStorage.h"


MessagesStorage::MessagesStorage()
{
	CreatePrepared();
}


MessagesStorage::~MessagesStorage()
{
	delete _PreparedAddMessage;
}

bool MessagesStorage::CreatePrepared()
{
	sql::Connection& Conn = Database::GetConnection();
	try
	{
		_PreparedAddMessage = Conn.prepareStatement("SELECT AddMessage(?,?,?,?);");
	}
	catch (sql::SQLException& e)
	{
		FallLog();
	}
	return true;
}


bool MessagesStorage::AddMessage(ID_t From, ID_t To, MessageContent Content)
{
	sql::SQLString str{(char*) Content.c_str(), Content.size() * sizeof(wchar_t) };

	Speakers sp{ From, To };
	boost::scoped_ptr<sql::ResultSet> _Result;
	try
	{
		_PreparedAddMessage->clearParameters();
		_PreparedAddMessage->setUInt(1, sp.first);
		_PreparedAddMessage->setUInt(2, sp.second);
		_PreparedAddMessage->setUInt(3, From);
		_PreparedAddMessage->setString(4, str);


		_Result.reset(_PreparedAddMessage->executeQuery());

		_Result->next();
		
		//sql returns 1 if chat is not created, if done
		return _Result->getInt(1) == 0;
	}
	//TODO: log + 
	catch (sql::SQLException &e) {
		FallLog();
	}
	return true;
}
