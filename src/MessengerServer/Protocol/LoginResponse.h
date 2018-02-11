#if !defined(LOGINRESPONSE_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#define LOGINRESPONSE_H

#include <Protocol\BaseHeader.h>

class LoginResponse : public BaseHeader 
{
public:
	LoginResponse(uint32_t Response = 0, ID_t id = 0);

    uint32_t ToBuffer(Byte* Buffery) const override;
    uint32_t FromBuffer(const Byte* Buffer, const size_t Capacity) override;

    uint32_t GetValue()const {
        return _Response;
    }
	ID_t GetId()const {
		return ID;
	}
	uint32_t CalculateSize() const {
		return BaseHeader::CalculateSize() +
			sizeof(LoginResponseDesc)
			- sizeof(BaseHeaderDesc);
	}
            
    enum ResponseValue
    {
        Success,
        Wrong,
        //account already online
        AccInUse,
        //connected already online
        AlreadyOnline,
        NoSuchUser
    };
protected:
    struct LoginResponseDesc{
        BaseHeaderDesc Header;
        uint32_t Response;
		ID_t ID;
	};
private:
    uint32_t _Response;
	ID_t ID;
};

#endif
