#pragma once

#include <GlobalInfo.h>
#include <string>
#include <TransferableType/Serializable.h>

class TextMessage
    : public Serializable<TextMessage, SerializableTypes::TextMessage>
{
public:
    using StorageType = std::wstring;

private:
    ID_t _From = 0;
    ID_t _To = 0;
    ID_t _InChatId = 0;
    //remove this later
    bool _Checked = false;
	StorageType _Message = {};

    struct DataMarking
    {
        uint32_t IdFrom;
        uint32_t IdTo;
        uint32_t Index;
        uint16_t MessageSize;
        alignas(4) BYTE Data[0];
    };
    static const size_t DataMarkingSize = sizeof(DataMarking);
        
public:
	TextMessage() {}
    TextMessage(ID_t from , ID_t to,ID_t in_chat_id, const std::wstring& message)
        :_From{ from }, _To{ to }, _InChatId{ in_chat_id },_Message {message} {}

    void SetChecked() { _Checked = true; }
    void SetUnchecked() { _Checked = false; }
    bool isChecked() const { return _Checked; }
    ID_t InChatId() const { return _InChatId; }
    ID_t From()const { return _From; }
    ID_t To()const { return _To; }
    const StorageType& GetBuffer() const { return _Message; }

    //implementation
    size_t _DataSize()const;
    bool _FromBuffer(const BYTE* Buffer, uint32_t recived);
    bool _ToBuffer(BYTE* Buffer)const;

};

