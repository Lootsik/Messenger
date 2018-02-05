#include <PacketFormat\Serialization.h>
#include <PacketFormat\Deserialization.h>

#include "Message.h"


bool Message::FromPacket(const BYTE* Buffer, uint32_t recived)
{
	return false;
}
bool Message::ToPacket(BYTE* Buffer) const
{
	return false;
}
