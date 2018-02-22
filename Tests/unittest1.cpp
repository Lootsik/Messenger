#include "stdafx.h"
#include "CppUnitTest.h"

#include <Protocol\Message.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			Message mes1{ 1,2,L"Hello World" };
			Byte Buff[512];
			mes1.ToBuffer(Buff);

			Message mes2;
			int err = mes2.FromBuffer(Buff, mes1.NeededSize());
			Assert::AreEqual(err,0,L"Error");



			Assert::IsTrue(mes1 == mes2, L"Not equal");
			// TODO: Your test code here
		}

	};
}