#pragma once

#include <Protocol\BaseHeader.h>


class LastMessageResponse: public BaseHeader
{
public:
	LastMessageResponse();
	LastMessageResponse(uint32_t id);

	uint32_t ToBuffer(Byte* Buffer)const override;
	uint32_t FromBuffer(const Byte* Buffer, const size_t Capacity) override;

	uint32_t GetId() const { return _MessageId; }


protected:
	struct LastMessageResponseDesc {
		BaseHeader Header;
		uint32_t MessageId;
	};
	uint32_t CalculateSize() const;
private:
	uint32_t _MessageId;
};
