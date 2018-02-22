#pragma once
#include <map>
#include <Network\Connection.h>
#include <Accounts\AccountManager.h>
#include <Messages\MessageManager.h>
#include <Protocol\GlobalInfo.h>
#include <Protocol\MessageRequest.h>
class Network;

//class Message;
class MessengerEngine
{
public:
	MessengerEngine(Network* server);
	AccountManager& Accounts(){	return _AccountManager;}
	/*	
	*/
	void AnalyzePacket(PConnection Connection);

	void _AutorizededProcess(PConnection& Connection);


	void OnLogin(PConnection& Connection);
	void OnLogout(PConnection& Connection);
	void OnUserInfo(PConnection& Connection);

	void OnMessage(PConnection& Connection);
	void OnMessageRequest(PConnection& Connection);


	
	void OnMessageRequest(PConnection& connection, MessageRequest& Request);
	void OnLastMessage(PConnection& connection, MessageRequest& Request);
	/*

	*/

	void SendResponce(PConnection& connection, const TransferredData& Result);
	//void SendMessageUser(Connection* connection, ID_t from, size_t messageNum, Message* mes);
	//void SendNewEventNotification(ID_t ID);
private:
	//packet already in Connection Writebuff
	//TODO: rewrite to abstract type
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
	MessageManager _MessageManager;
	Network* _Server;
};
