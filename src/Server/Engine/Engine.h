#pragma once
#include <Network/Connection.h>
#include <Accounts/AccountStorage/AccountStorage.h>
#include <Accounts/AccountManager.h>
#include <GlobalInfo.h>
#include <boost/shared_ptr.hpp>
class Network;

//class Message;
class MessengerEngine
{
public:
	MessengerEngine(Network* server);
	bool LoadFromConfig(const char* Filename);
	AccountManager& Accounts(){	return _AccountManager;}
	/*	
			Parse
	*/
	void AnalyzePacket(boost::shared_ptr<Connection> Connection);
	void OnLogin(Connection& Connection);
	void OnLogout(Connection& Connection);
	void OnMessage(Connection& Connection);
	
	/*

	*/

	//void SendMessageUser(boost::shared_ptr<Connection>> connection, ID_t from, size_t messageNum, Message* mes);
	//void SendNewEventNotification(ID_t ID);
private:
	void SendLoginResponce(Connection& connection, const LoginResponse& Result);
	//packet already in Connection Writebuff
	template<typename T>
	bool _MakePacket(Connection& connection,const T& Item)
	{
		uint32_t err = Item.ToBuffer(connection->WriteBuf());
		if ( err )
			return false;
	
		connection->BytesWrite = Item.NeededSize();
		return true;
	}
	void _Send(Connection& connection);
	AccountManager _AccountManager;
	Network* _Server;
};
