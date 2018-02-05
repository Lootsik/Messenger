#include "Chat.h"

void Chat::AddMessage(ID_t from, std::wstring& message)
{
	Messages.push_back(new Message{ from, false,message });
}
Message* Chat::GetUserMessage(size_t messI)
{
	if (messI >= Messages.size())
		throw;

	return Messages[messI];
}

size_t Chat::LastMessageIndex()
{
	return Messages.size();
}