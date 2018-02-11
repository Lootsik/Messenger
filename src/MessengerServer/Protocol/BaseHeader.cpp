/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Vitaliy Lutsyshyn $
   ======================================================================== */
#include <Protocol\SerializationError.h>
#include "BaseHeader.h"

uint32_t BaseHeader::HeaderCheck(const BaseHeaderDesc* data, const size_t size)const
{
    if( data->FrameSize == size)
        return SerializationError::Ok;
    else
        return SerializationError::InvalidHeader;
}

uint32_t BaseHeader::ToBuffer(Byte* Buffer)const
{
    BaseHeaderDesc* Data = (BaseHeaderDesc*) Buffer;
    *Data = TData;
    return SerializationError::Ok;
}

uint32_t BaseHeader::FromBuffer(const Byte* Buffer, const size_t  Capacity)
{
    const BaseHeaderDesc* Data = (BaseHeaderDesc*)Buffer;
	uint32_t err = HeaderCheck(Data, Capacity);
    if(err ){
        return SerializationError::InvalidData;
    }

    TData = *Data;
    return SerializationError::Ok;
}

