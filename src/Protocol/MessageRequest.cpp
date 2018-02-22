/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Vitaliy Lutsyshyn $
   ======================================================================== */
#include <Protocol\Types.h>
#include <Protocol\SerializationError.h>

#include "MessageRequest.h"


MessageRequest::MessageRequest
			(uint32_t RequestType, uint32_t From, uint32_t To, uint32_t Index )
	: BaseHeader(Types::MessageRequest, sizeof(MessageRequestDesc)),
	_RequestType{ RequestType }, _From{ From }, _To{ To }, _MessageIndex{Index}
{
    SetFrameSize( CalculateSize() );
}

MessageRequest::MessageRequest()
        : BaseHeader( Types::MessageRequest,sizeof(MessageRequestDesc))
{
    SetFrameSize( CalculateSize());
}


uint32_t MessageRequest::CalculateSize() const
{
    return sizeof(MessageRequestDesc);
}


uint32_t MessageRequest::ToBuffer(Byte* Buffer) const
{
    uint32_t err = BaseHeader::ToBuffer(Buffer);
    if( err )
        return err;

	MessageRequestDesc* Request = (MessageRequestDesc*)Buffer;
	Request->Type = _RequestType;
	Request->From= _From;
	Request->To = _To;
	Request->MessageIndex = _MessageIndex;

    return  SerializationError::Ok;
}


uint32_t MessageRequest::FromBuffer(const Byte* Buffer, const size_t Capacity)
{
	uint32_t err = BaseHeader::FromBuffer(Buffer, Capacity);
    if(err)
        return err;

	MessageRequestDesc* Request = (MessageRequestDesc*)Buffer;
    _RequestType = Request->Type;   
	_From = Request->From ;
	_To = Request->To;
	_MessageIndex = Request->MessageIndex;
    
	return  SerializationError::Ok;
}


