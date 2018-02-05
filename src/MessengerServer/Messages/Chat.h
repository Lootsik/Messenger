#pragma once
#include <vector>
#include "Message.h"
//fist member less that second
class Chat
{
	ID_t ID;
	//only two for now
	ID_t Members[2];
	size_t LastUnchecked_1;
	size_t LastUnchecked_2;
	
public:
	std::vector<Message*> Messages;
	Chat(ID_t ChatId = INVALID_ID, ID_t Member1 = INVALID_ID, ID_t Member2 = INVALID_ID) :ID{ ChatId }, Members{ Member1,Member2 } {};
	void AddMessage(ID_t from, std::wstring& message);
	Message* GetUserMessage(size_t messI);
	size_t LastMessageIndex();
};
