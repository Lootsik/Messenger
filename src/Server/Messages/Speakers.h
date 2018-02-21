#pragma once
#include <utility>
#include <GlobalInfo.h>

//to snach overloaded operators
class Speakers : public std::pair<ID_t, ID_t>
{
public:

	//save less than greater value
	Speakers(ID_t user1, ID_t user2) 
				: std::pair<ID_t, ID_t>{ user1, user2  }
	{
		if (user2 < user1)
			std::swap(first, second);
	}
};