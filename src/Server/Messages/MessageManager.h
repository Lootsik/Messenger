#pragma once
#include <Protocol\GlobalInfo.h>

#include <Accounts\AccountManager.h>

#include <Protocol\Types\Message.h>
#include <Protocol\Types\LastMessageResponse.h>
#include <Protocol\Types\Message.h>

#include <Messages\MessagesStorage.h>

class Engine;

class MessageManager
{
public:
	MessageManager(AccountManager& manager) :_AccountManager{ manager } {}


	bool PostMessageUser(ID_t ID_from, ID_t ID_to, const std::wstring& Content);
	Message GetMessageUser(ID_t ID_from, ID_t ID_to, uint32_t  index);

	//INVALID_ID if no messages in chat or no such chat
	LastMessageResponse GetLastMessageID(ID_t ID_from, ID_t ID_to, ID_t another);

	static const unsigned MessageMax = Storing::MessageSizeMax;

private:
	MessagesStorage _MessageStorage;
	AccountManager& _AccountManager;
};