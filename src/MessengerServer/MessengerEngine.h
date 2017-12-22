#pragma once
#include <map>
#include "Clients.h"
#include "Database.h"
struct Server;

#define INVALID_ID 0

class MessengerEngine
{
public:
	const char* ConfigFilename = "Messenger.cfg";

	MessengerEngine(Server* server);
	~MessengerEngine();
	//изменить сигнатуру
	bool Login(Client* client, const std::string& entered_login, const std::string& entered_password);
	void Logout(Client* client);
	//void Message(Client* client);
	void AnalyzePacket(Client* client, size_t size);
	
	bool SettingsFromConfig();

	//void Response(Client* client);


private:
	std::map <uint32_t, Account*> _Accounts;
	Database _DB;
	Server* _Server;

	uint32_t CheckAccount(const std::string& entered_login, const std::string& entered_password);

};

