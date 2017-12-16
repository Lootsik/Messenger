#pragma once
#include <map>
#include "Clients.h"
#include "Database.h"
class Server;

class MessengerEngine
{
public:
	MessengerEngine(Server* server);
	~MessengerEngine();
	//mores
	//изменить сигнатуру
	void Login(Client* client, const std::string& entered_login, const std::string& entered_password);
	void Logout(Client* client);
	//void Message(Client* client);

private:
	std::map <std::string, Account*> _Accounts;
	Database _DB;
	Server* _Server;

	bool _PasswordCheck(const Account* account, const std::string& entered_login, const std::string& entered_password);
};

