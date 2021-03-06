#if !defined(TRANSFERREDDATA_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   ======================================================================== */

#define TRANSFERREDDATA_H

#include <cstdint>

class TransferredData
{
public:
	virtual uint32_t GetType() const = 0;
    virtual uint32_t NeededSize()const = 0;    
    virtual uint32_t ToBuffer(Byte* Buffery)const = 0;
    virtual uint32_t FromBuffer(const Byte* Buffer,const size_t Capacity) = 0;
    virtual ~TransferredData() {};
};

#endif
