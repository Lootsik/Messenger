#pragma once

#include <Protocol\BaseHeader.h>


class LastMessageResponse: public BaseHeader
{
public:
	LastMessageResponse();
	LastMessageResponse(ID_t anotherID, uint32_t messageid);

	// 			 Override
	uint32_t ToBuffer(Byte* Buffer)const override;
	uint32_t FromBuffer(const Byte* Buffer, const size_t Capacity) override;

	//			 Getters 
	uint32_t GetMessageId() const { return _MessageId; }
	uint32_t GetAnotherId() const { return _AnotherUserId; }

	bool operator==(const LastMessageResponse& rv)const {
		if (BaseHeader::operator==(rv)) {
			return _MessageId == rv._MessageId;
		}
		return false;
	}

protected:
	struct LastMessageResponseDesc {
		BaseHeader Header;
		ID_t AnotherId;
		uint32_t MessageId;
	};
	uint32_t CalculateSize() const;
private:
	uint32_t _MessageId;
	uint32_t _AnotherUserId;
};
