#pragma once
#include <unordered_map>
#include <functional>

#include <Network\Connection.h>
#include <Accounts\AccountManager.h>
#include <Messages\MessageManager.h>
#include <Protocol\GlobalInfo.h>
#include <Protocol\Types.h>


class Network;

class MessengerEngine;
using MemFn = std::function<void(MessengerEngine*, PConnection&, TransferredData*)>;

//class Message;
class MessengerEngine
{
public:
	MessengerEngine(Network* server);

	AccountManager& Accounts(){	return _AccountManager;}
	
	void AnalyzePacket(PConnection Connection);
	void ForceLogout(PConnection& connection);

private:
	void _AutorizededProcess(PConnection& Connection, TransferredData* Data);


	void OnLogin(PConnection& Connection, TransferredData* Data);
	void OnLogout(PConnection& Connection, TransferredData* Data);
	void OnUserInfo(PConnection& Connection, TransferredData* Data);

	void OnMessage(PConnection& Connection, TransferredData* Data);
	void OnMessageRequest(PConnection& Connection, TransferredData* Data);


	void OnMessageRequest_M(PConnection& connection, MessageRequest& Request);
	void OnLastMessage(PConnection& connection, MessageRequest& Request);


	void SendResponce(PConnection& connection, const TransferredData& Result);
	
	//void SendMessageUser(Connection* connection, ID_t from, size_t messageNum, Message* mes);
	//packet already in Connection Writebuff
	//TODO: rewrite to abstract type
	template<typename T>
	bool __MakePacket(PConnection& connection,const T& Item)
	{
		uint32_t err = Item.ToBuffer(connection->WriteBuffer().c_array());
		if ( err )
			return false;
	
		connection->SetBytesToWrite( Item.NeededSize() );
		return true;
	}
	bool _MakePacket(PConnection& connection, const TransferredData& Item);
	void _Send(PConnection& connection);


	AccountManager _AccountManager;
	MessageManager _MessageManager;
	Network* _Server;

	const std::unordered_map<int, MemFn> UnauthorizedOperations;
	const std::unordered_map<int, MemFn> AuthorizedOperations;

};
