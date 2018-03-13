/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Vitaliy Lutsyshyn $
   ======================================================================== */

#include <Protocol\Types.h>
#include <Protocol\SerializationError.h>

#include "Message.h"

static uint32_t CalculateBufferSize(const std::wstring& str)
{
	return static_cast<uint32_t>(str.size()) * sizeof(wchar_t);
}


Message::Message()
			:BaseHeader(Types::Message, sizeof(MessageResponseDesc))
{
    SetFrameSize(CalculateSize());
}


Message::Message(uint32_t Sender, uint32_t Reciver, const std::wstring& Content, uint32_t MessageIndex)
			:BaseHeader(Types::Message, sizeof(MessageResponseDesc)),
			_Sender{ Sender }, _Reciver{ Reciver }, _Message {Content},
			_MessageIndex{ MessageIndex }
{
	SetFrameSize(CalculateSize());
}


uint32_t Message::CalculateSize() const
{
	return sizeof(MessageResponseDesc) +
				CalculateBufferSize(_Message);
}

//check
uint32_t Message::MessageResponseCheck(const MessageResponseDesc* Buffer, const size_t size)const
{
	uint32_t Result = HeaderCheck(&(Buffer->Header), size);
	if (Result)
		return Result;

	if (Buffer->Header.FrameSize !=
			Buffer->MessageSize + 
			sizeof(MessageResponseDesc))
		Result = SerializationError::InvalidData;

	return Result;
}



uint32_t Message::ToBuffer(Byte* Buffer) const
{
	uint32_t err = BaseHeader::ToBuffer(Buffer);
	if (err)
		return err;

	MessageResponseDesc* Response = (MessageResponseDesc*)Buffer;

	Response->MessageIndex = _MessageIndex;
	Response->Sender = _Sender;
	Response->Reciver = _Reciver;
	Response->MessageSize = CalculateBufferSize(_Message);

	memcpy(Response->Message, _Message.c_str(), Response->MessageSize);

	return  SerializationError::Ok;
}
uint32_t Message::FromBuffer(const Byte* Buffer, const size_t Capacity)
{
	const MessageResponseDesc* Response = (const MessageResponseDesc*)Buffer;

	uint32_t err = MessageResponseCheck(Response, Capacity);
	if (err)
		return err;

	err = BaseHeader::FromBuffer(Buffer, Capacity);
	if (err)
		return err;

	_MessageIndex = Response->MessageIndex;
	_Sender = Response->Sender;
	_Reciver = Response->Reciver;

	//becouse it takes lenght not in bytes, but in wchar_t
	_Message = { (wchar_t*)Response->Message,Response->MessageSize /sizeof(wchar_t) };
	return SerializationError::Ok;
}

