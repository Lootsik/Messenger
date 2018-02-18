#pragma once
#include <Protocol\BaseHeader.h>
#include <string>

class UserInfo :
	public BaseHeader
{

public:
	UserInfo(uint32_t id, const std::string& Login);
	UserInfo();

	~UserInfo() {}

	uint32_t ToBuffer(Byte* Buffer)const override;
	uint32_t FromBuffer(const Byte* Buffer, const size_t Capacity) override;

	const std::string& GetLogin() const { return _Login; }
	ID_t GetId() const { return _ID; }

protected:

#pragma warning( push )
#pragma warning( disable : 4200 )

	struct UserInfoDesc {
		BaseHeaderDesc Header;
		ID_t ID;
		uint32_t LoginSize;
		Byte Data[0];
	};

#pragma warning( pop )

	uint32_t CalculateSize() const;
	//check
	uint32_t UserInfoCheck(const UserInfoDesc* Buffer, const size_t size)const;
private:
	std::string _Login;
	ID_t _ID = 0;
};

