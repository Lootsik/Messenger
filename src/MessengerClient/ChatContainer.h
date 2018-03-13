#pragma once
#include <string>
#include <vector>

#include <API/MessengerAPI.h>
struct ChatContainer
{
	using Message_t = std::pair < ID_t, std::wstring>;
	
	ID_t another = 0;
	uint32_t MessCount= 0;

	std::vector<Message_t> Messages;


	uint32_t LoadedMessages = 0;
	void AddMessage(ID_t index, ID_t sender, const std::wstring& message)
	{
		// TODO: remove
		if (index > MessCount)
			throw "Wrong Index";

		Messages[index - 1] = { sender, message };

		++LoadedMessages;
	}
	bool AllLoaded()
	{
		return MessCount == LoadedMessages;
	}
};

