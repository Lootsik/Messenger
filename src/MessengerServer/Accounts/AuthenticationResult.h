#pragma once
#include <TransferableType/Serializable.h>

class AuthenticationResult 
			:public Serializable<AuthenticationResult, SerializableTypes::AuthenticationResult> {

	struct Marking
	{
		uint32_t Result;
		uint32_t ID;
	};
	static const size_t MarkingSize = sizeof(Marking);
	uint32_t _Result;
	ID_t _ID;
public:
	enum Result
	{
		Success,
		Wrong,
		//account already online
		AccInUse,
		//connected already online
		AlreadyOnline,
		NoSuchUser
	};

	AuthenticationResult(uint32_t Result, ID_t ID = INVALID_ID) 
					:_Result{ Result }, _ID{ ID } {}

	uint32_t GetValue()const {	return _Result;	}
	ID_t GetId() const { return _ID; }

	size_t _DataSize()const {return MarkingSize;}

	bool _FromBuffer(const BYTE* Buffer, uint32_t recived);
	bool _ToBuffer(BYTE* Buffer)const;
};