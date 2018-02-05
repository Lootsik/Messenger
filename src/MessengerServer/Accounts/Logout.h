#pragma once
#include <TransferableType/Serializable.h>

class Logout : public Serializable<Logout, SerializableTypes::Logout>
{
public:
	size_t _DataSize()const { return 0; }

	bool _FromBuffer(const BYTE* Buffer, uint32_t recived) { return true; }
	bool _ToBuffer(BYTE* Buffer)const { return true; }
};

