#if !defined(LOGOUT_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define LOGOUT_H

#include <Protocol\BaseHeader.h>

class Logout: public BaseHeader
{
    Logout();

    uint32_t ToBuffer(Byte* Buffer)const override;
    uint32_t FromBuffer(const Byte* Buffer, const size_t Capacity) override;

protected:
    uint32_t CalculateSize() const;
};

#endif
