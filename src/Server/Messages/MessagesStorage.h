#pragma once

#pragma warning(push)
#pragma warning( disable : 4251 )
#include <cppconn\exception.h>
#pragma warning( pop )

#include <Protocol\GlobalInfo.h>

#include <Utility\InPlace.h>

#include <Database\Database.h>

#include <Messages\Speakers.h>



// if error with databasee occur, sql::SQLException will throw
class MessagesStorage : InPlace
{
public:
	MessagesStorage();
	~MessagesStorage();
	
	enum class State {
		Ok = 0,
		NoSuchMessage,
		NoSuchChat,
		DataTooBig,
	};
	
	
	// new message
	State AddMessage(ID_t From, ID_t To, const MessageContent& Content);



	// get existing message
	State LoadMessage(const Speakers& speakers, ID_t InChatId, std::pair<ID_t, MessageContent>& Message);

	
	// equal to all messages in chat
	ID_t GetLastMessageID(const Speakers& speakers);


private:
	
	bool CreatePrepared();


	sql::PreparedStatement* _PreparedAddMessage;
	sql::PreparedStatement* _PreparedGetMessage;
	sql::PreparedStatement* _PreparedGetChat;
	sql::PreparedStatement* _PreparedCreateChat;

	// no size checks
	State _TryAddMessage(ID_t From, ID_t To, const std::string& str);


	// if no such chat, INVALID_ID returned
	ID_t _GetChat(const Speakers& speakers);



	void _CreateChat(const Speakers& speakers);
};

