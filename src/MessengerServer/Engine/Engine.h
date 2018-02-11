#pragma once
#include <map>
#include <Server/Connection.h>
#include <Accounts/AccountStorage/AccountStorage.h>
#include <Accounts/AccountManager.h>
#include <GlobalInfo.h>
class Server;

//class Message;
class MessengerEngine
{
public:
	MessengerEngine(Server* server);
	bool LoadFromConfig(const char* Filename);
	AccountManager& Accounts(){	return _AccountManager;}
	/*	
			Parse
	*/
	void AnalyzePacket(Connection* Connection);
	void OnLogin(Connection* Connection);
	void OnLogout(Connection* Connection);
	void OnMessage(Connection* Connection);
	
	/*

	*/


	void SendLoginResponce(Connection* connection, const LoginResponse& Result);
	//void SendMessageUser(Connection* connection, ID_t from, size_t messageNum, Message* mes);
	//void SendNewEventNotification(ID_t ID);
private:
	//packet already in Connection Writebuff
	template<typename T>
	bool _MakePacket(Connection* connection,const T& Item)
	{
		if (!Item.ToBuffer(connection->WriteBuf()))
			return false;
	
		connection->BytesWrite = Item.NeededSize();
		return true;
	}
	void _Send(Connection* connection);
	AccountManager _AccountManager;
	Server* _Server;
};
