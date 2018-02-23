#pragma once
#include <Protocol\BaseHeader.h>

class MessageRequest: public BaseHeader
{
public:
	MessageRequest(uint32_t RequestType, uint32_t From, uint32_t To, uint32_t Index = 0);
	MessageRequest();

	// 			Overrided
    uint32_t ToBuffer(Byte* Buffer)const override;
    uint32_t FromBuffer(const Byte* Buffer, const size_t Capacity) override;

    //			Getters
	uint32_t GetRequestType()const { return _RequestType; }
	uint32_t Sender()const { return _From; }
	uint32_t Receiver() const { return _To; }
	uint32_t GetMessageIndex() const { return _MessageIndex; }

    //TODO: add get 
	enum RequestType
	{
		Message,
		LastMessageID
    };

	bool operator==(const MessageRequest& rv)const {
		if (BaseHeader::operator==(rv)) {
			return _RequestType == rv._RequestType && 
				   _From ==  rv._From &&
				   _To == rv._To &&
				   _MessageIndex == rv._MessageIndex;
		}
		return false;
	}

protected:

#pragma warning( push )
#pragma warning( disable : 4200 )

    //TODO: add fields
    struct MessageRequestDesc
    {
		BaseHeaderDesc Header;

		uint32_t Type;
        uint32_t From;
		uint32_t To;

		uint32_t MessageIndex;
    };

#pragma warning( pop ) 

    uint32_t CalculateSize() const;

private:
    uint32_t _RequestType = 0;
	uint32_t _From = 0;
	uint32_t _To = 0;
	uint32_t _MessageIndex = 0;
};

