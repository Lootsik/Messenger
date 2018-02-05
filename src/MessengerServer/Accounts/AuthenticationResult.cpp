#include "AuthenticationResult.h"

bool AuthenticationResult::_FromBuffer(const BYTE* Buffer, uint32_t recived)
{
	if (MarkingSize < recived)
		return false;

	Marking* marking = (Marking*)Buffer;
	_Result = marking->Result;
	_ID = marking->ID;
	return true;
}
bool AuthenticationResult::_ToBuffer(BYTE* Buffer)const
{
	Marking* marking = (Marking*)Buffer;
	marking->Result = _Result;
	marking->ID = _ID;
	return true;
}