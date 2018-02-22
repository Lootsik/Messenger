#pragma once
#include <utility>
#include <Protocol\GlobalInfo.h>

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
	uint32_t GetAnother(ID_t user)
	{
		if (user == first)
			return second;
		else
			return first;
	}
};