/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Vitaliy Lutsyshyn $
   ======================================================================== */
#include <Protocol\Types.h>
#include "Logout.h"

Logout::Logout():
		BaseHeader(Types::Logout,sizeof( BaseHeaderDesc))
{
    SetFrameSize(CalculateSize());
}

uint32_t Logout::CalculateSize()const
{
    return BaseHeader::CalculateSize();
}

uint32_t Logout::ToBuffer(Byte* Buffer)const
{
    uint32_t err = BaseHeader::ToBuffer(Buffer);
    return err;
}

uint32_t Logout::FromBuffer(const Byte* Buffer, const size_t Capacity)
{
    uint32_t err = BaseHeader::FromBuffer(Buffer,Capacity);
    return err;
}
