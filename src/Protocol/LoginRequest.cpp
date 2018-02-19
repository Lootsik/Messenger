/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Vitaliy Lutsyshyn $
   ======================================================================== */

#include <Protocol\Types.h>
#include <Protocol\SerializationError.h>

#include "LoginRequest.h"

LoginRequest::LoginRequest(const std::string& Login, const std::string& Password)
        :BaseHeader(Types::LoginRequest,sizeof(LoginRequestDesc)),_Login{Login},_Password{Password}
{
    SetFrameSize(CalculateSize());
}

LoginRequest::LoginRequest()
		:BaseHeader(Types::LoginRequest, sizeof(LoginRequestDesc))
{
	SetFrameSize(CalculateSize());
}
//checks rules for data size
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

uint32_t LoginRequest::CalculateSize() const
{
    uint32_t res = sizeof(LoginRequestDesc);
    res += static_cast<uint32_t>( _Login.size() + _Password.size());
	return res;
}

uint32_t LoginRequest::ToBuffer(Byte* Buffer)const
{
	uint32_t err = BaseHeader::ToBuffer(Buffer);
    if( err )
        return err;

    LoginRequestDesc* AuthData = (LoginRequestDesc*)Buffer;

    AuthData->LoginSize = static_cast<uint32_t>(_Login.size());
    AuthData->PasswordSize = static_cast<uint32_t>(_Password.size());

    memcpy(AuthData->Data, _Login.c_str(), AuthData->LoginSize );
    memcpy(AuthData->Data + AuthData->LoginSize , _Password.c_str(), AuthData->PasswordSize );
    return SerializationError::Ok;
}

uint32_t LoginRequest::FromBuffer(const Byte* Buffer, size_t Capacity) 
{
    const LoginRequestDesc* AuthData = (LoginRequestDesc*)Buffer;
    uint32_t err = LoginRequestCheck(AuthData, Capacity);
    if(err)
        return err;

    err = BaseHeader::FromBuffer(Buffer, Capacity);
    if(err)
        return err;

    uint32_t LoginSize = AuthData->LoginSize;
    uint32_t PasswordSize = AuthData->PasswordSize;
    _Login = std::string{ AuthData->Data , AuthData->Data + LoginSize };
    _Password = std::string{AuthData->Data + LoginSize, AuthData->Data + LoginSize + PasswordSize };

    return SerializationError::Ok;
}
    
