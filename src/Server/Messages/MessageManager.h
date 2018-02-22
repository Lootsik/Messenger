#pragma once
#include <Protocol\GlobalInfo.h>

#include <Messages\MessagesStorage.h>
#include <Accounts\AccountManager.h>
#include <Protocol\Message.h>
#include <Protocol\LastMessageResponse.h>
#include <Protocol\Message.h>
//just interface for now
class Engine;
struct MessageManager
{
	MessageManager(AccountManager& manager) :_AccountManager{ manager } {}


	void PostMessageUser(ID_t ID_from, ID_t ID_to, const std::wstring& Content);
	Message GetMessageUser(ID_t ID_from, ID_t ID_to, uint32_t  index);

	//INVALID_ID if no messages in chat or no such chat
	LastMessageResponse GetLastMessageID(ID_t ID_from, ID_t ID_to);


private:
	MessagesStorage _MessageStorage;
	AccountManager& _AccountManager;
};