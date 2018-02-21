#pragma once

#include <GlobalInfo.h>
#include <Utility\InPlace.h>
#include <Database\Database.h>

#include <Messages\Speakers.h>

class MessagesStorage
{
public:
	MessagesStorage();
	~MessagesStorage();
	
	bool CreatePrepared();


	bool AddMessage(ID_t From, ID_t To, MessageContent Content);

	MessageContent GetMessage(const Speakers& speakers, ID_t InChatId);

	// amount messages in chat
	ID_t GetLastMessageID(const Speakers& speakers);
	/*
	GetMessage();
	SendMessage();
	GetLenght();
	*/

private:
	sql::PreparedStatement* _PreparedAddMessage;

//  ChatExist();
	//SELECT id FROM chats where Member1 = 7 and Member2 = 8;

//	AddChat();
	//INSERT INTO chats(Member1, Member2) VALUES(8, 9);

//	DeleteChat();
};

