/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Vitaliy Lutsyshyn $
   ======================================================================== */
#include <Protocol\Types.h>
#include <Protocol\SerializationError.h>

#include "RegistrationResponse.h"

RegistrationResponse::RegistrationResponse(uint32_t Response)
        :BaseHeader(Types::RegistrationResponse,sizeof(RegistrationResponseDesc)), _Response{Response}
{
    SetFrameSize(CalculateSize());
}
uint32_t RegistrationResponse::ToBuffer(Byte* Buffer) const
{
    uint32_t err = BaseHeader::ToBuffer(Buffer);
    if( err )
        return err;
    
	((RegistrationResponseDesc*)Buffer)->Response = _Response;
    return  SerializationError::Ok;
}
uint32_t RegistrationResponse::FromBuffer(const Byte* Buffer, const size_t Capacity)
{
	uint32_t err = BaseHeader::FromBuffer(Buffer,Capacity);
	if (err)
		return err;

	_Response = ((RegistrationResponseDesc*)Buffer)->Response;
    return SerializationError::Ok;
}
