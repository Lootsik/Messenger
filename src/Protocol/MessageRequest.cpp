/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Vitaliy Lutsyshyn $
   ======================================================================== */
#include <Protocol\Types.h>
#include <Protocol\SerializationError.h>

#include "MessageReuqset.h"


MessageRequest::MessageRequest(uint32_t RequestType)
        : BaseHeader( Types::MessageRequest,sizeof(MessageRequestDesc)),
          _RequestType{RequestType}
{
    SetFrimeSize( CalculateSize());
}

MessageRequest::MessageRequest()
        : BaseHeader( Types::MessageRequest,sizeof(MessageRequestDesc))
{
    SetFrimeSize( CalculateSize());
}


uint32_t MessageRequest::CalculateSize() const
{
    return sizeof(LoginRequestDesc);
}


uint32_t ToBuffer(Byte* Buffer)const 
{
    uint32_t err = BaseHeader::ToBuffer(Buffer);
    if( err )
        return err;

    MessageRequestDesc* Request = (MessageRequestDesc*)Buffer;
    Request->Type = _RequestType;

    return  SerializationError::Ok;
}


uint32_t FromBuffer(const Byte* Buffer, const size_t Capacity)
{
    err = BaseHeader::FromBuffer(Buffer, Capacity);
    if(err)
        return err;

    _RequestType = Request->Type;   
    
    return  SerializationError::Ok;
}


