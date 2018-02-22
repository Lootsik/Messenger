#include <Engine\Engine.h>
#include "MessageManager.h"


LastMessageResponse MessageManager::GetLastMessageID(ID_t ID_from, ID_t ID_to)
{
	Speakers speakers(ID_from, ID_to);

	return { _MessageStorage.GetLastMessageID(speakers) };
}


Message MessageManager::GetMessageUser(ID_t ID_from, ID_t ID_to, uint32_t  index)
{
	Speakers speakers(ID_from, ID_to);

	auto Res = _MessageStorage.LoadMessage(speakers, index);
	return { Res.first, speakers.GetAnother(Res.first),Res.second, index };
}

void MessageManager::PostMessageUser(ID_t ID_from, ID_t ID_to, const std::wstring& Content)
{
	_MessageStorage.AddMessage(ID_from, ID_to, Content);
}