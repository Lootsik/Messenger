#include "stdafx.h"
#include "CppUnitTest.h"

#include <Protocol\Message.h>
#include <Protocol\MessageRequest.h>
#include <Protocol\LoginResponse.h>
#include <Protocol\UserInfo.h>
#include <Protocol\LoginRequest.h>
#include <Protocol\LastMessageResponse.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

template<typename T, typename... Args>
void TestPacket(Args... args)
{
	T mes1{ args... };
	Byte Buff[512];
	mes1.ToBuffer(Buff);

	T mes2;
	int err = mes2.FromBuffer(Buff, mes1.NeededSize());
	Assert::AreEqual(err, 0, L"Error");


	Assert::IsTrue(mes1 == mes2, L"Not equal");
}

namespace Tests
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(Message_)
		{
			
			TestPacket<Message>(1u, 2u, L"Message");
		}
		TEST_METHOD(MessageRequest_)
		{
			TestPacket<MessageRequest>(1u, 2u, 3u,4u);
		}
		TEST_METHOD(UserInfo_)
		{
			TestPacket<UserInfo>(1u,"");
		}
		TEST_METHOD(LoginResponse_)
		{
			TestPacket<LoginResponse>(1u, 2u);
		}
		
		TEST_METHOD(LoginRequest_)
		{
			TestPacket<LoginRequest>("Hello", "world");
		}
		TEST_METHOD(LastMessageResponse_)
		{
			TestPacket<LastMessageResponse>(1u);
		}

	};
}