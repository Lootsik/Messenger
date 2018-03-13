#pragma once
#include <memory>

#include <Protocol\TransferredData.h>
#include <Protocol\BaseHeader.h>

class NetworkStrategy
{
public:
	virtual bool Connect(const std::string& Address, unsigned short port) = 0;

	virtual bool Ready() = 0;
	virtual BaseHeader* GetPacket() = 0;

	virtual bool Send(const TransferredData& Data) = 0;

	virtual void Release() = 0;

	virtual ~NetworkStrategy() {};

	class __ConnectionRefused {};
	class __ConnectionDropped {};
};

