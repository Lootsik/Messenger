#include "stdafx.h"
#include "CppUnitTest.h"

#include <Protocol\Types.h>


#include <thread>

#include <API/Query.h>
#include <ctime>
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

	TEST_CLASS(QueryTest)
	{
	public:
		TEST_METHOD(AsyncQuery)
		{
			Query<std::string> query;
			time_t start = time(NULL);
			std::thread first{ [&]() {
				while (1)
				{
					query.push_back("Hello world");
					auto time_now = time(NULL);
					if (time_now - start > 2)
						break;
				}
			} };
			std::thread second{ [&]() {
				while (1)
				{
					if (time(NULL) - start > 3)
					{
						while (query.ready())
							query.pop_front();
						break;
					}
					int truies_left = 10;
					while (!query.ready())
					{
						truies_left--;
						if (! truies_left)
							break;
					};
					if (!truies_left)
						continue;

					query.pop_front();
					auto time_now = time(NULL);

				
				}
			} };
			first.join();
			second.join();
			Assert::IsFalse(query.ready());
		}
	};
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
			TestPacket<LastMessageResponse>(1u,2u);
		}

	};
}