#pragma once

#include <Protocol\BaseHeader.h>
#include <string>

class Message: public BaseHeader
{
public:
	Message( uint32_t Sender, uint32_t Reciver, 
			const std::wstring& Content, uint32_t MessageIndex = 0);
	Message();
    
    // 				Overrided
    uint32_t ToBuffer(Byte* Buffer)const override;
    uint32_t FromBuffer(const Byte* Buffer, const size_t Capacity) override;

    // 				Getters
	uint32_t MessageIndex()const { return _MessageIndex; }
	uint32_t Sender() const { return _Sender; }
	uint32_t Receiver() const { return _Reciver; }
	const std::wstring& Content() const { return _Message; }

	bool operator==(const Message& rv)const {
		if (BaseHeader::operator==(rv)) {
			return _MessageIndex == rv._MessageIndex &&
				   _Sender == rv._Sender &&
				   _Reciver == rv._Reciver && 
				   _Message == rv._Message;
		}
		return false;
	}
    
protected:

#pragma warning( push )
#pragma warning( disable : 4200 )

	struct MessageResponseDesc {
		BaseHeaderDesc Header;

		uint32_t MessageIndex;
		uint32_t Sender;
		uint32_t Reciver;
		uint32_t MessageSize;
		
		Byte Message[0];
    };

#pragma warning( pop ) 

    uint32_t CalculateSize() const;
    //			Check
    uint32_t MessageResponseCheck(const MessageResponseDesc* Buffer, const size_t size)const;

private:
	uint32_t _MessageIndex = 0;
	uint32_t _Sender = 0;
	uint32_t _Reciver = 0;
	std::wstring _Message;
};  
