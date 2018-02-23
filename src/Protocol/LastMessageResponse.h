#pragma once

#include <Protocol\BaseHeader.h>


class LastMessageResponse: public BaseHeader
{
public:
	LastMessageResponse();
	LastMessageResponse(uint32_t id);

	// 			 Override
	uint32_t ToBuffer(Byte* Buffer)const override;
	uint32_t FromBuffer(const Byte* Buffer, const size_t Capacity) override;

	//			 Getters 
	uint32_t GetId() const { return _MessageId; }

	bool operator==(const LastMessageResponse& rv)const {
		if (BaseHeader::operator==(rv)) {
			return _MessageId == rv._MessageId;
		}
		return false;
	}

protected:
	struct LastMessageResponseDesc {
		BaseHeader Header;
		uint32_t MessageId;
	};
	uint32_t CalculateSize() const;
private:
	uint32_t _MessageId;
};
