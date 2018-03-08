#include <Protocol\BaseHeader.h>
#include <Protocol\Types.h>

#include <Protocol\TransferredData.h>
#include <Protocol\LoginRequest.h>
#include <Protocol\LoginResponse.h>
#include <Protocol\Types.h>
#include <Protocol\UserInfo.h>
#include <Protocol\Message.h>
#include <Protocol\MessageRequest.h>
#include <Protocol\LastMessageResponse.h>

#include "PacketAnalyzer.h"

PacketAnalyzer::PacketAnalyzer()
{
}

BaseHeader* PacketAnalyzer::Analyze(const Byte* Packet, size_t size)
{

	switch (BaseHeader::BufferType(Packet))
	{

	case Types::LoginResponse:
	{

		printf("LoginResponse\n");
		LoginResponse* Data = new LoginResponse;
		uint32_t err = Data->FromBuffer(Packet, size);
		if (err)
		{
			printf("Error when unpack\n");
			return  nullptr ;

		}
		return  Data ;
		
	}break;

	case Types::UserInfo:
	{

		UserInfo* Data = new UserInfo;
		uint32_t err = Data->FromBuffer(Packet, size);
		if (err)
		{
			printf("Error when unpack\n");
			return  nullptr ;

		}
		return Data ;

	}break;


	case Types::Message:
	{
		printf("Message\n");

		Message* Data = new Message;
		uint32_t err = Data->FromBuffer(Packet, size);
		if (err)
		{
			printf("Error when unpack\n");
			return  nullptr ;

		}
		return  Data ;

	}
	case Types::LastMessageResponse:
	{

		LastMessageResponse* Data = new LastMessageResponse;
		uint32_t err = Data->FromBuffer(Packet, size);
		if (err)
		{
			printf("Error when unpack\n");
			return  nullptr ;
		}
		return Data;
	}

	}
	return  nullptr ;
}


PacketAnalyzer::~PacketAnalyzer()
{
}
