#pragma once
#include <GlobalInfo.h>
#include <map>
#include <Messages\Chat.h>
#include <Accounts\AccountManager.h>
#include <Server\Server.h>
//dummy for now
class Engine;
struct MessageManager
{
	const ID_t SingleChat = 1;

	MessageManager(MessengerEngine& engine):_Engine{engine}
	{
		Chats[SingleChat] = Chat(SingleChat, 9, 7);
	}
	void SendMessageUser(ID_t ID_from, ID_t ID_to, std::wstring Mes);
	void GetMessageUser(Connection* connection, size_t index);
	//_Send message
	//Get Message
private:
	MessengerEngine & _Engine;
	std::map<ID_t, Chat> Chats;
};