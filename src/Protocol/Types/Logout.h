#if !defined(LOGOUT_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   ======================================================================== */

#define LOGOUT_H

#include <Protocol\BaseHeader.h>

class Logout: public BaseHeader
{
public:
    Logout();

    //            Getters
    uint32_t ToBuffer(Byte* Buffer)const override;
    uint32_t FromBuffer(const Byte* Buffer, const size_t Capacity) override;

    bool operator==(const Logout& rv)const {
        return BaseHeader::operator==(rv);
    }


protected:
    uint32_t CalculateSize() const;
};

#endif
