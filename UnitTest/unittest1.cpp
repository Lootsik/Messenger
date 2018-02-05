#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
#include "../src/MessengerServer/TransferableType/TransferObject.h"
#include "../src/MessengerServer/GlobalInfo.h"
namespace UnitTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			BYTE Buffer[PacketMaxSize];
			TransferObject Trans;
			Trans.ToBuffer(Buffer);

		}

	};
}