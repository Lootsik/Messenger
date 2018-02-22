#include <Database\DbInternals.h>

#include <Logger\Logger.h>

#include "MessagesStorage.h"

//TODO: make "atomic" in database

MessagesStorage::MessagesStorage()
{
	CreatePrepared();
}


MessagesStorage::~MessagesStorage()
{
	delete _PreparedGetMessage;
	delete _PreparedAddMessage;
	delete _PreparedGetChat;
	delete _PreparedCreateChat;
}

bool MessagesStorage::CreatePrepared()
{
	sql::Connection& Conn = Database::GetConnection();
	try
	{
		_PreparedAddMessage = Conn.prepareStatement("SELECT AddMessage(?,?,?,?);");
		_PreparedGetMessage = Conn.prepareStatement("SELECT Content FROM messages WHERE Chat = ? and IdInChat = ? ;");
		_PreparedGetChat = Conn.prepareStatement("SELECT id FROM chats WHERE Member1 = ? and Member2 = ? ;");
		_PreparedCreateChat  = Conn.prepareStatement("INSERT INTO chats(Member1,Member2) VALUES(?,?);");
	}
	catch (sql::SQLException& e)
	{
		FallLog();
	}
	return true;
}

bool MessagesStorage::AddMessage(ID_t From, ID_t To, const MessageContent& Content)
{
	if (!_TryAddMessage(From, To, Content))
	{
		Speakers speakers{ From, To };
		
		_CreateChat(speakers);

		ID_t ChatId = _GetChat(speakers);
		if (ChatId == INVALID_ID)
		{
			//TODO: fix
			throw;
		}
		return _TryAddMessage(From, To, Content);
	}

	return true;
}


// mostly inmplemented in sql function
// if false, need create chat and repeat
// if true - done
bool MessagesStorage::_TryAddMessage(ID_t From, ID_t To, const MessageContent& Content)
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

//TODO: return sender
std::pair<ID_t, MessageContent> MessagesStorage::LoadMessage(const Speakers& speakers, ID_t InChatId)
{
	ID_t ChatId = _GetChat(speakers);
	if (ChatId == INVALID_ID)
	{
		return {};
		//report err
	}

	boost::scoped_ptr<sql::ResultSet> _Result;
	try
	{
		_PreparedGetMessage->clearParameters();
		_PreparedGetMessage->setUInt(1, ChatId);
		_PreparedGetMessage->setUInt(2, InChatId);
	
		_Result.reset(_PreparedGetMessage->executeQuery());

		if (_Result->next()) 
		{
			ID_t sender = _Result->getUInt("SenderId");
			std::string str = _Result->getString("Content");
			return { sender,{ (wchar_t*)str.c_str(),str.size() / sizeof(wchar_t)} };
		}
		
	}
	//TODO: log + 
	catch (sql::SQLException &e) {
		FallLog();
	}
	return {};
}

//TODO: do smth with return return vlaue if no chat
ID_t MessagesStorage::GetLastMessageID(const Speakers& speakers)
{
	ID_t id = _GetChat(speakers);
	if (id == INVALID_ID)
		return INVALID_ID;

	return Database::GetStatement().execute("SELECT LastMessageId FROM chats WHERE id = " + std::to_string(id) + " ;");
}

// if no such chat, INVALID_ID returned
ID_t MessagesStorage::_GetChat(const Speakers& speakers)
{
	boost::scoped_ptr<sql::ResultSet> _Result;
	try
	{
		_PreparedGetChat->clearParameters();
		_PreparedGetChat->setUInt(1, speakers.first);
		_PreparedGetChat->setUInt(2, speakers.second);

		_Result.reset(_PreparedGetChat->executeQuery());

		if (_Result->next())
			return _Result->getUInt(1);
	}
	//TODO: log + 
	catch (sql::SQLException &e) {
		FallLog();
	}

	return INVALID_ID;
}

void MessagesStorage::_CreateChat(const Speakers& speakers)
{
	try
	{
		_PreparedCreateChat->clearParameters();
		_PreparedCreateChat->setUInt(1, speakers.first);
		_PreparedCreateChat->setUInt(2, speakers.second);

		_PreparedCreateChat->execute();

	}
	//TODO: log + 
	catch (sql::SQLException &e) {
		FallLog();
	}
}