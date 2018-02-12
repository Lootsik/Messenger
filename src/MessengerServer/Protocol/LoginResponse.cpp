/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Vitaliy Lutsyshyn $
   ======================================================================== */
#include <Protocol\Types.h>
#include <Protocol\SerializationError.h>

#include "LoginResponse.h"

LoginResponse::LoginResponse(uint32_t Response,ID_t id)
        :BaseHeader(Types::LoginResponse,sizeof(LoginResponseDesc)), _Response{Response},ID{id}
{
    SetFrameSize(CalculateSize());
}
uint32_t LoginResponse::ToBuffer(Byte* Buffer) const
{
    uint32_t err = BaseHeader::ToBuffer(Buffer);
    if( err )
        return err;
    
	((LoginResponseDesc*)Buffer)->Response = _Response;
    return  SerializationError::Ok;
}
uint32_t LoginResponse::FromBuffer(const Byte* Buffer, const size_t Capacity)
{
	uint32_t err = BaseHeader::FromBuffer(Buffer,Capacity);
	if (err)
		return err;

	((LoginResponseDesc*)Buffer)->Response = _Response;
    return SerializationError::Ok;
}
