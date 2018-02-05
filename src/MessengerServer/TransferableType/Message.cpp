#include "Message.h"


bool TextMessage::FromPacket(const BYTE* Buffer, uint32_t recived)
{
    if( !TransferableType::FromBuffer(Buffer, recived))
        return false;

    return _InputData( GetAdditionalData(Buffer),AdditionalDataSize());
}


//bool ToPacket(BYTE* Buffer)const;    
