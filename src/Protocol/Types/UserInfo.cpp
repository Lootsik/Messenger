
#include <Protocol\Types.h>
#include <Protocol\SerializationError.h>

#include "UserInfo.h"

UserInfo::UserInfo(uint32_t id, const std::string& Login)
			:BaseHeader(Types::UserInfo, sizeof(UserInfoDesc)), _ID{ id }, _Login{ Login } 
{
	SetFrameSize(CalculateSize());
}


UserInfo::UserInfo()
			:BaseHeader(Types::UserInfo, sizeof(UserInfoDesc))
{
	SetFrameSize(CalculateSize());
}


uint32_t UserInfo::CalculateSize() const
{
	uint32_t res = sizeof(UserInfoDesc);
	res += static_cast<uint32_t>(_Login.size());
	return res;
}
//check
uint32_t UserInfo::UserInfoCheck(const UserInfoDesc* Buffer, const size_t size)const
{
	uint32_t err = HeaderCheck(&(Buffer->Header), size);
	if (err)
		return err;

	if (Buffer->Header.FrameSize != 
				sizeof(UserInfoDesc) + Buffer->LoginSize)
		return err = SerializationError::InvalidData;

	return SerializationError::Ok;
}


uint32_t UserInfo::ToBuffer(Byte* Buffer)const
{
	uint32_t err = BaseHeader::ToBuffer(Buffer);
	if (err)
		return err;

	UserInfoDesc* Users = (UserInfoDesc*)Buffer;

	Users->ID = _ID;
	Users->LoginSize = static_cast<uint32_t>(_Login.size());
	
	memcpy(Users->Data, _Login.c_str(), Users->LoginSize);
	return SerializationError::Ok;
}

uint32_t UserInfo::FromBuffer(const Byte* Buffer, size_t Capacity)
{
	const UserInfoDesc* Users = (UserInfoDesc*)Buffer;
	uint32_t err = UserInfoCheck(Users, Capacity);
	if (err)
		return err;

	err = BaseHeader::FromBuffer(Buffer, Capacity);
	if (err)
		return err;

	_ID = Users->ID;
	uint32_t LoginSize = Users->LoginSize;
	_Login = std::string{ Users->Data , Users->Data + LoginSize };

	return SerializationError::Ok;
}

