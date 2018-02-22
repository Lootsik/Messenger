#if !defined(BASEHEADER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define BASEHEADER_H


#include <Protocol\GlobalInfo.h>
#include <Protocol\TransferredData.h>

class BaseHeader : public TransferredData {

public:
    //check, that size is valid for packet
    static bool MinimumCheck(size_t size) {
        return size >= sizeof(BaseHeaderDesc);
    }

    static uint32_t BufferType(const Byte* Buffer){
        return ((BaseHeaderDesc*)Buffer)->Type;
    }
    /*
            core 
     */
    uint32_t NeededSize() const override{
        return TData.FrameSize;
    }
    uint32_t ToBuffer(Byte* Buffery) const override;
    uint32_t FromBuffer(const Byte* Buffer, const size_t Capacity) override;

    //          Getters 
    uint32_t GetFrameSize() const {
        return TData.FrameSize;
    }
    uint32_t GetType()const override {
        return TData.Type;
    }
    ~BaseHeader() {}
    
protected:
    // data markup 
    struct BaseHeaderDesc{
        uint32_t FrameSize = 0;
        uint32_t Type = 0;
    };
    //in derived classes to initalize base,
    //we need pass type in constructor,
    //next call SetFrameSize with
    //return of directly called class::CalculateSize  
    BaseHeader(uint32_t type, uint32_t MinTypeFrameSize) 
                    : TData{ 0,type }, _MinTypeFrameSize{ MinTypeFrameSize } {}
    
    /*
      used in construction derived objects
     */
    void SetFrameSize(uint32_t size){
        TData.FrameSize = size;
    }
    uint32_t CalculateSize() const {
        return sizeof(BaseHeaderDesc);
    }
    //when derived dont need her own checks, not necessary use this
    //frombuffer uses this aswell 
    //useful to override in derived classes if they have
    //variable lenght arrays
    uint32_t HeaderCheck(const BaseHeaderDesc* data, const size_t size)const;

private:
    //. transferred data
    BaseHeaderDesc TData;
    //used in header check
    uint32_t _MinTypeFrameSize;
};
#endif
