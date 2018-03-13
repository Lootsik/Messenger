#include <Protocol\Types.h>
#include <Protocol\SerializationError.h>

#include "LastMessageResponse.h"


LastMessageResponse::LastMessageResponse()
			:BaseHeader(Types::LastMessageResponse, sizeof(LastMessageResponseDesc))
{
	SetFrameSize(CalculateSize());
}


LastMessageResponse::LastMessageResponse(ID_t anotherID, uint32_t id)
			:BaseHeader(Types::LastMessageResponse, sizeof(LastMessageResponseDesc)),
				_MessageId{ id }, _AnotherUserId{anotherID}
{
	SetFrameSize(CalculateSize());
}


uint32_t LastMessageResponse::CalculateSize() const
{
	return sizeof(LastMessageResponseDesc);
}


uint32_t LastMessageResponse::ToBuffer(Byte* Buffer)const
{
	uint32_t err = BaseHeader::ToBuffer(Buffer);
	if (err)
		return err;

	LastMessageResponseDesc* Request = (LastMessageResponseDesc*)Buffer;
	Request->AnotherId = _AnotherUserId;
	Request->MessageId = _MessageId;

	return  SerializationError::Ok;
}


uint32_t LastMessageResponse::FromBuffer(const Byte* Buffer, const size_t Capacity)
{
	uint32_t err = BaseHeader::FromBuffer(Buffer, Capacity);
	if (err)
		return err;

	LastMessageResponseDesc* Request = (LastMessageResponseDesc*)Buffer;
	_AnotherUserId = Request->AnotherId;
	_MessageId = Request->MessageId;

	return  SerializationError::Ok;
}
