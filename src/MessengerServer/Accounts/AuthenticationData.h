#if !defined(TRANSFEROBJECT_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#define TRANSFEROBJECT_H

#include <TransferableType/Serializable.h>
#include <string>
class AuthenticationData : public Serializable<AuthenticationData, SerializableTypes::AuthenticationData>{
	
	std::string _Login{};
	std::string _Password{};

	struct DataMarking
	{
		uint16_t LoginSize;
		uint16_t PassSize;
		alignas(4) BYTE Data[0];
	};
	
	static const size_t DataPartHeaderSize = sizeof(DataMarking);
	//checks
	bool _ValidPacket(const uint16_t PacketSize, const uint16_t LoginLen, const uint16_t PassLen);
	bool _EnoughSpace(const size_t LoginSize, const size_t PassSize, uint16_t Size);

public: 
	AuthenticationData() {}
	AuthenticationData(const std::string& login, const std::string& password )
				:_Login{ login }, _Password{ password } {}
	
	const std::string& GetLogin() const {return _Login;}	
	const std::string& GetPassword() const {return _Password;}
	//how much bytes buffer needed to fit serialized data
	size_t _DataSize()const;

	bool _FromBuffer(const BYTE* Buffer, uint32_t recived);
	bool _ToBuffer(BYTE* Buffer)const;
};

#endif
