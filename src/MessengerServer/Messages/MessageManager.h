#pragma once
#include <GlobalInfo.h>
#include <unordered_map>
#include <Messages\Chat.h>
#include <Accounts\AccountManager.h>
#include <Server\Server.h>

//just interface for now
class Engine;
struct MessageManager
{
	MessageManager(MessengerEngine& engine) :_Engine{ engine } {}
	void SendMessageUser(ID_t ID_from, ID_t ID_to, std::wstring Mes);
	void GetMessageUser(Connection* connection, size_t index);
	//_Send message
	//Get Message
private:
	MessengerEngine & _Engine;
};