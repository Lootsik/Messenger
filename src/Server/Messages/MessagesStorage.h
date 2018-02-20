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

	bool AddMessage(ID_t From, ID_t To, MessageContent Content);
	MessageContent GetMessage(const Speakers& speakers, ID_t InChatId);

	// amount messages in chat
	uint32_t GetLenght(const Speakers& speakers);
	/*
	GetMessage();
	SendMessage();
	GetLenght();
	*/

private:


//  ChatExist();
//	AddChat();
//	DeleteChat();
};

