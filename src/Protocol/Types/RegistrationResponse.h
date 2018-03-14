#if !defined(REGISTRATIONRESPONSE_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   ======================================================================== */
#define REGISTRATIONRESPONSE_H

#include <Protocol\BaseHeader.h>

class RegistrationResponse : public BaseHeader 
{
public:
    RegistrationResponse(uint32_t Response = 0);

    uint32_t ToBuffer(Byte* Buffery) const override;
    uint32_t FromBuffer(const Byte* Buffer, const size_t Capacity) override;
    
    //              Getters
    uint32_t GetValue()const {return _Response; }
            
	enum ResponseValue
	{
		Success,
		Denied,
		NotAllowedLogin,
		LoginUsed,
    };

    bool operator==(const RegistrationResponse& rv)const {
        if (BaseHeader::operator==(rv)) {
			return _Response == rv._Response;
		}
        return false;
    }


protected:
    struct RegistrationResponseDesc{
		BaseHeaderDesc Header;
		uint32_t Response;
    };

    uint32_t CalculateSize() const {
        return sizeof(RegistrationResponseDesc);
    }
private:
    uint32_t _Response;
};

#endif
