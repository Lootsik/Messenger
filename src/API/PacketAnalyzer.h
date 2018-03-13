#pragma once
#include <memory>

#include <Protocol\GlobalInfo.h>
#include <Protocol\TransferredData.h>


class PacketAnalyzer
{
public:
	PacketAnalyzer();
	BaseHeader* Analyze(const Byte* Packet, size_t size);
	~PacketAnalyzer();
};

