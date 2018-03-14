/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Vitaliy Lutsyshyn $
   ======================================================================== */

#include <Protocol\Types.h>
#include <Protocol\SerializationError.h>

#include "RegistrationRequest.h"

RegistrationRequest::RegistrationRequest(const std::string& Login, const std::string& PasswordHash)
	:BaseHeader(Types::RegistrationRequest, sizeof(RegistrationRequestDesc)), _Login{ Login }, _PasswordHash{ PasswordHash }
{
	SetFrameSize(CalculateSize());
}

RegistrationRequest::RegistrationRequest()
		:BaseHeader(Types::RegistrationRequest, sizeof(RegistrationRequestDesc))
{
	SetFrameSize(CalculateSize());
}
//checks rules for data size
uint32_t RegistrationRequest::RegistrationRequestCheck( const RegistrationRequestDesc* Buffer, const size_t size)const
{
    uint32_t Result = HeaderCheck(&(Buffer->Header),size);
    if( Result)
        return Result;

    if( Buffer->Header.FrameSize !=
            Buffer->LoginSize  + Storing::PasswordHashSize +
                 sizeof( RegistrationRequestDesc ))
        Result = SerializationError::InvalidData;

    return Result;
}

uint32_t RegistrationRequest::CalculateSize() const
{
    uint32_t res = sizeof(RegistrationRequestDesc);
    res += static_cast<uint32_t>( _Login.size() + Storing::PasswordHashSize);
	return res;
}

uint32_t RegistrationRequest::ToBuffer(Byte* Buffer)const
{
	if (_PasswordHash.size() != Storing::PasswordHashSize)
		return SerializationError::InvalidData;

	uint32_t err = BaseHeader::ToBuffer(Buffer);
    if( err )
        return err;

    RegistrationRequestDesc* AuthData = (RegistrationRequestDesc*)Buffer;

    AuthData->LoginSize = static_cast<uint32_t>(_Login.size());

    memcpy(AuthData->Data, _Login.c_str(), AuthData->LoginSize );
    memcpy(AuthData->Data + AuthData->LoginSize , _PasswordHash.c_str(), Storing::PasswordHashSize);
    return SerializationError::Ok;
}

uint32_t RegistrationRequest::FromBuffer(const Byte* Buffer, size_t Capacity) 
{
    const RegistrationRequestDesc* AuthData = (RegistrationRequestDesc*)Buffer;
    uint32_t err = RegistrationRequestCheck(AuthData, Capacity);
    if(err)
        return err;

    err = BaseHeader::FromBuffer(Buffer, Capacity);
    if(err)
        return err;

    uint32_t LoginSize = AuthData->LoginSize;
  
    _Login = std::string{ AuthData->Data , AuthData->Data + LoginSize };
    _PasswordHash = std::string{AuthData->Data + LoginSize, AuthData->Data + LoginSize + Storing::PasswordHashSize };

    return SerializationError::Ok;
}
    
