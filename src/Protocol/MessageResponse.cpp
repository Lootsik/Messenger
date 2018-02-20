/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Vitaliy Lutsyshyn $
   ======================================================================== */

#include <Protocol\Types.h>
#include <Protocol\SerializationError.h>

#include "MessageResponse.h"


MessageResponse(uint32_t Response,MessageType& Message)
        :BaseHeader(Types::MessageResponse, sizeof(LoginRequestDesc)),
             _Presponse{Response},_Message{Message}
{
    SetFrameSize(CalculateSize());
}


MessageResponse()
       :BaseHeader(Types::MessageResponse, sizeof(LoginRequestDesc))
{
    SetFrameSize(CalculateSize());
}

//TODO: wtire this
uint32_t LoginRequest::LoginRequestCheck( const LoginRequestDesc* Buffer, const size_t size)const
{
    uint32_t Result = HeaderCheck(&(Buffer->Header),size);
    if( Result)
        return Result;

    if( Buffer->Header.FrameSize !=
            Buffer->LoginSize + Buffer->PasswordSize +
                 sizeof( LoginRequestDesc ))
        Result = SerializationError::InvalidData;

    return Result;
}

//TODO: write
uint32_t LoginRequest::CalculateSize() const
{
    uint32_t res = sizeof(LoginRequestDesc);
    res += static_cast<uint32_t>( _Login.size() + _Password.size());
    return res;
}



    
uint32_t ToBuffer(Byte* Buffer)const override;
uint32_t FromBuffer(const Byte* Buffer, const size_t Capacity) override;

