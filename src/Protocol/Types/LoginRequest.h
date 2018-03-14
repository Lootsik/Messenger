#if !defined(LOGINREQUES_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#define LOGINREQUES_H

#include <string>

#include <Protocol\BaseHeader.h>

class LoginRequest: public BaseHeader
{
public:
    LoginRequest(const std::string& Login, const std::string& PasswordHash);
	LoginRequest();

    //              Overrided
    uint32_t ToBuffer(Byte* Buffer)const override;
    uint32_t FromBuffer(const Byte* Buffer, const size_t Capacity) override;

    //              Getters
    const std::string& GetLogin()const{
        return _Login;
    }
    const std::string& GetPasswordHash()const{
        return _PasswordHash;
    }

    bool operator==(const LoginRequest& rv)const {
        if (BaseHeader::operator==(rv)) {
            return _Login == rv._Login &&
				_PasswordHash == rv._PasswordHash;
            }
        return false;
    }
    
protected:

#pragma warning( push )
#pragma warning( disable : 4200 )

    struct LoginRequestDesc{
        BaseHeaderDesc Header;
        uint32_t LoginSize;
		Byte Data[0];
	};

#pragma warning( pop ) 

    uint32_t CalculateSize() const;
	//     check
	uint32_t LoginRequestCheck(const LoginRequestDesc* Buffer, const size_t size)const;
private:
    //LoginRequestDesc LRData;
    std::string _Login;
    std::string _PasswordHash;
};
#endif
