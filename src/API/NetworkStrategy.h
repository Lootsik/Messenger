#pragma once
#include <string>

#include <Protocol\TransferredData.h>

class NetworkStrategy
{
public:
	virtual bool Connect(const std::string& Address, unsigned short port) = 0;

	virtual bool Ready() = 0;
	virtual TransferredData* GetPacket() = 0;

	virtual bool Send(const TransferredData& Data) = 0;

	virtual void Release() = 0;

	virtual ~NetworkStrategy() {};

	class __ConnectionRefused {};
	class __ConnectionDropped {};
};

