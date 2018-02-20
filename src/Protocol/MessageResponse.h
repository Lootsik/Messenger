#if !defined(MESSAGERESPONSE_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define MESSAGERESPONSE_H

#include <string>

#include <Protocol\BaseHeader.h>

class MessageResponse: public BaseHeader{

public:
    MessageResponse(uint32_t Response,MessageType& Message);
    MessageResponse();
    
    uint32_t ToBuffer(Byte* Buffer)const override;
    uint32_t FromBuffer(const Byte* Buffer, const size_t Capacity) override;

    using MessageType = std::wstring;

    MessageType& GetMessage() const { return _Message;}
}
    
protected:

#pragma warning( push )
#pragma warning( disable : 4200 )

    struct MessageResponseDesc{
        BaseHeaderDesc Header;
        uint32_t Response;
        union {
            uint32_t AdditionalInfo;
            Byte Message[0];
        };
    };

#pragma warning( pop ) 

    uint32_t CalculateSize() const;
    //check
    uint32_t MessageResponseCheck(const MessageResponseDesc* Buffer, const size_t size)const;
private:
    //MessageResponseDesc;
    uint32_t Response = 0;
    MessageType _Message;    
};  






#endif
