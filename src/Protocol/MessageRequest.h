#if !defined(MESSAGEREQUEST_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#define MESSAGEREQUEST_H

#include <Protocol\BaseHeader.h>

struct ChatData;

class MessageRequest: public BaseHeader
{

public:
    MessageRequest(uint32_t RequestType);
    MessageRequest();

    uint32_t ToBuffer(Byte* Buffer)const override;
    uint32_t FromBuffer(const Byte* Buffer, const size_t Capacity) override;

    //TODO: add get 
    enum RequestType
    {
        GetMessage
        

    };
    
    
protected:

#pragma warning( push )
#pragma warning( disable : 4200 )

    //TODO: add fields
    struct MessageRequestDesc
    {
        uint32_t Type;
        ChatData* data;
               
        Byte Data[0];
    };

#pragma warning( pop ) 

    uint32_t CalculateSize() const;

private:
    uint32_t _ReuqestType;
        
};

#endif
