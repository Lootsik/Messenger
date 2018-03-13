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
		_PreparedGetMessage = Conn.prepareStatement("SELECT SenderId, Content FROM messages WHERE Chat = ? and IdInChat = ? ;");
		_PreparedGetChat = Conn.prepareStatement("SELECT id FROM chats WHERE Member1 = ? and Member2 = ? ;");
		_PreparedCreateChat  = Conn.prepareStatement("INSERT INTO chats(Member1,Member2) VALUES(?,?);");
	}
	catch (sql::SQLException& e)
	{
		FallLog(e);
	}
	return true;
}


MessagesStorage::State MessagesStorage::AddMessage(ID_t From, ID_t To, const MessageContent& Content)
{
	std::string ByteString{ (char*)Content.c_str(), Content.size() * sizeof(wchar_t) };

	auto ret = _TryAddMessage(From, To, ByteString);
	
	if (ret == State::NoSuchChat)
	{
		Speakers speakers{ From, To };
		
		_CreateChat(speakers);

		ID_t ChatId = _GetChat(speakers);
		if (ChatId == INVALID_ID)
			// internal error 
			throw;

		return _TryAddMessage(From, To, ByteString);
	}

	return State::Ok;
}

// mostly inmplemented in sql function
// if false, need create chat and repeat
// if true - done
MessagesStorage::State MessagesStorage::_TryAddMessage(ID_t From, ID_t To, const std::string& str)
{
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
		if (_Result->getInt(1) == 0)
			return State::Ok;
		else 
			return State::NoSuchChat;
	}
	catch (sql::SQLException &e) {

		// data too long
		if (e.getErrorCode() == 1406)
			return State::DataTooBig;
		
		FallLog(e);
	}
}

MessagesStorage::State MessagesStorage::LoadMessage(const Speakers& speakers, ID_t InChatId, std::pair<ID_t, MessageContent>& Message)
{
	ID_t ChatId = _GetChat(speakers);
	if (ChatId == INVALID_ID)
		return State::NoSuchMessage;

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
			Message.first = sender;
			Message.second = { (wchar_t*)str.c_str(),str.size() / sizeof(wchar_t) };

			return State::Ok;
		}
		
	}
	catch (sql::SQLException &e) {
		FallLog(e);
	}
	return State::NoSuchMessage;
}

ID_t MessagesStorage::GetLastMessageID(const Speakers& speakers)
{
	ID_t id = _GetChat(speakers);
	if (id == INVALID_ID)
		return INVALID_ID;
	
	try
	{
		boost::scoped_ptr<sql::ResultSet> _Result{
				Database::GetStatement().executeQuery("SELECT LastMessageId FROM chats WHERE id = " 
						+ std::to_string(id) + " ;") };
	
		if (_Result->next())
			return _Result->getUInt(1);
		
	}
	catch (sql::SQLException &e) {
		FallLog(e);
	}
	return INVALID_ID;
}

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
	catch (sql::SQLException &e) {
		FallLog(e);
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
	catch (sql::SQLException &e) {
		FallLog(e);
	}
}