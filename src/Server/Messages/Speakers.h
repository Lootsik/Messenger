#pragma once
#include <GlobalInfo.h>

class Speakers {
	ID_t _User[2];
public:
	Speakers(ID_t user1, ID_t user2) :_User{ user1,user2 }
	{
		if (user2 > user1){
			_User[0] = user2;
			_User[1] = user1;
		}
	}
	const ID_t&   operator[](int Index)const  {
		return _User[Index];
	}
};