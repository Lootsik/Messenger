#if !defined(REGISTRATIONREQUES_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   ======================================================================== */
#define REGISTRATIONREQUES_H

#include <string>

#include <Protocol\BaseHeader.h>

class RegistrationRequest: public BaseHeader
{
public:
    RegistrationRequest(const std::string& Login, const std::string& PasswordHash);
	RegistrationRequest();

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

    bool operator==(const RegistrationRequest& rv)const {
        if (BaseHeader::operator==(rv)) {
            return _Login == rv._Login &&
				_PasswordHash == rv._PasswordHash;
            }
        return false;
    }
    
protected:

#pragma warning( push )
#pragma warning( disable : 4200 )

    struct RegistrationRequestDesc{
        BaseHeaderDesc Header;
        uint32_t LoginSize;
		Byte Data[0];
	};

#pragma warning( pop ) 

    uint32_t CalculateSize() const;
	//     check
	uint32_t RegistrationRequestCheck(const RegistrationRequestDesc* Buffer, const size_t size)const;
private:
    //RegistrationRequestDesc LRData;
    std::string _Login;
    std::string _PasswordHash;
};
#endif
