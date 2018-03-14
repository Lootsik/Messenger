#if !defined(TYPES_H)
#define TYPES_H

#include <Protocol\Types\UserInfo.h>
#include <Protocol\Types\MessageRequest.h>
#include <Protocol\Types\Message.h>
#include <Protocol\Types\LastMessageResponse.h>
#include <Protocol\Types\LoginRequest.h>
#include <Protocol\Types\LoginResponse.h>
#include <Protocol\Types\Logout.h>
#include <Protocol\Types\RegistrationRequest.h>
#include <Protocol\Types\RegistrationResponse.h>

namespace Types
{
uint32_t FromBuffer(const Byte* Buffer, const size_t Capacity, TransferredData*& Transferable);

	enum Types {
		NotType = 0,
		PrevOperationDenied,
		UserInfo,
		MessageRequest,
		Message,
		LastMessageResponse,
		LoginRequest,
		LoginResponse,
		Logout,
		RegistrationRequest,
		RegistrationResponse,
	};
}

#endif
