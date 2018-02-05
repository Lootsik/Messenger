#pragma once
#include <GlobalInfo.h>
#include <Messages\Speakers.h>

class MessageRequest
{
	Speakers _Speakers;
	ID_t _Index;
	uint32_t _Type;
public:
	MessageRequest(const Speakers& speakers, ID_t index,uint32_t type)
		: _Speakers{ speakers }, _Index{ index }, _Type{ type } {}

	ID_t Index() const { return _Index; }
	Speakers& const Speakers() { return _Speakers; }
	uint32_t Type() { return _Type; }

	bool ToBuffer(BYTE* Buffer)const;
	bool FromBuffer(const BYTE* Buffer, const uint32_t recived );
};
