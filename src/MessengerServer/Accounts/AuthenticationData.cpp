#include "AuthenticationData.h"

bool AuthenticationData::_FromBuffer(const BYTE* Buffer, uint32_t recived)
{
	DataMarking* marking = ((DataMarking*)Buffer);
	uint16_t LoginLen = marking->LoginSize;
	uint16_t PassLen = marking->PassSize;

	if (!_ValidPacket(recived, LoginLen, PassLen))
		return false;

	_Login = std::string{ marking->Data , marking->Data + LoginLen };
	_Password = std::string{ marking->Data + LoginLen , marking->Data + LoginLen + PassLen };

	return true;
}
// Buffer already have needed capacity
bool AuthenticationData::_ToBuffer(BYTE* Buffer)const
{
	size_t LoginSize = _Login.size();
	size_t PassSize = _Password.size();

	DataMarking* marking = (DataMarking*)Buffer;

	marking->LoginSize = LoginSize;
	marking->PassSize = PassSize;
	//copying string

	memcpy(marking->Data, _Login.c_str(), marking->LoginSize);
	memcpy(marking->Data + marking->LoginSize, _Password.c_str(), marking->PassSize);

	return true;
}


bool AuthenticationData::_ValidPacket(const uint16_t PacketSize, const uint16_t LoginLen, const uint16_t PassLen)
{
	return PacketSize == DataPartHeaderSize + LoginLen + PassLen;
}
bool AuthenticationData::_EnoughSpace(const size_t LoginSize, const size_t PassSize, uint16_t Size)
{
	return  Size >= LoginSize + PassSize + DataPartHeaderSize;
}

size_t AuthenticationData::_DataSize()const 
{
	return _Login.size() + _Password.size() + DataPartHeaderSize;
}