#include <PacketFormat\Serialization.h>
#include <PacketFormat\Deserialization.h>
#include "Message.h"

static size_t BufferSize(const std::wstring& str )
{
    return str.size() * sizeof(wchar_t);
}



size_t TextMessage:: _DataSize()const
{
    return DataMarkingSize + BufferSize(_Message); 
}

bool TextMessage::_FromBuffer(const BYTE* Buffer, uint32_t recived)
{
    DataMarking* marking = (DataMarking*)Buffer;

    uint16_t MessageSize = marking->MessageSize;
    //check, valid buffer on not
    if( recived == MessageSize + DataMarkingSize)
        return false;

    _From = marking->IdFrom;
    _To = marking->IdTo;
    _InChatId = marking->Index;
    
    _Message = {marking->Data, marking->Data + MessageSize };
    return true;
}

bool TextMessage::_ToBuffer(BYTE* Buffer) const
{
    DataMarking* marking = (DataMarking*)Buffer;

    marking->IdFrom = _From;
    marking->IdTo = _To;
    marking->Index = _InChatId;
    //its safe, checked by Serializable
    marking->MessageSize = _Message.size();
        
    memcpy(marking->Data, _Message.c_str(), marking->MessageSize);
    return false;
}
