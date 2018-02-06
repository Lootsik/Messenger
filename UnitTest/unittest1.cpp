#include "stdafx.h"
#include "CppUnitTest.h"
#include <Messages\Message.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(Message_Check)
		{
			TextMessage message{ 7,8,1, L"Hey hey hey!" };
			BYTE Buffer[512];
			message.ToBuffer(Buffer);

			TextMessage message2;
			message2.FromBuffer(Buffer, message.GetSerializedSize());

			Assert::IsTrue(message.To() == message2.To()
							&& message.From() == message2.From()
							&& message.GetType() == message2.GetType()
							&& message.GetSerializedSize() == message2.GetSerializedSize());;
		}

	};
}