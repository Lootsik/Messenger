#pragma once
#include <map>
#include <Network/Connection.h>
#include <Accounts/AccountStorage/AccountStorage.h>
#include <Accounts/AccountManager.h>
#include <GlobalInfo.h>
class Network;

//class Message;
class MessengerEngine
{
public:
	MessengerEngine(Network* server);
	bool LoadFromConfig(const char* Filename);
	AccountManager& Accounts(){	return _AccountManager;}
	/*	
	*/
	void AnalyzePacket(PConnection Connection);
	void OnLogin(PConnection& Connection);
	void OnLogout(PConnection& Connection);
	void OnMessage(PConnection& Connection);
	
	/*

	*/

	void SendLoginResponce(PConnection& connection, const LoginResponse& Result);
	//void SendMessageUser(Connection* connection, ID_t from, size_t messageNum, Message* mes);
	//void SendNewEventNotification(ID_t ID);
private:
	//packet already in Connection Writebuff
	template<typename T>
	bool _MakePacket(PConnection& connection,const T& Item)
	{
		uint32_t err = Item.ToBuffer(connection->WriteBuffer().c_array());
		if ( err )
			return false;
	
		connection->SetBytesToWrite( Item.NeededSize() );
		return true;
	}
	void _Send(PConnection& connection);
	AccountManager _AccountManager;
	Network* _Server;
};
