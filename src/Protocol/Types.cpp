#include <map>
#include <functional>

#include "Types.h"

template<typename _Transferable>
TransferredData* ConstructTransferable()
{
	return new _Transferable;
}


using ConstructFunc = std::function<TransferredData*(void)>;

static const std::map<uint32_t, ConstructFunc> TypeHandlers = {
			{ Types::UserInfo, ConstructTransferable<UserInfo>},
			{ Types::MessageRequest, ConstructTransferable<MessageRequest> },
			{ Types::Message, ConstructTransferable<Message> },
			{ Types::LastMessageResponse, ConstructTransferable<LastMessageResponse> },
			{ Types::LoginRequest, ConstructTransferable<LoginRequest> },
			{ Types::LoginRequest, ConstructTransferable<LoginRequest> },
			{ Types::LoginResponse, ConstructTransferable<LoginResponse> },
			{ Types::Logout, ConstructTransferable<Logout> },
			{ Types::RegistrationRequest, ConstructTransferable<RegistrationRequest> },
			{ Types::RegistrationResponse, ConstructTransferable<RegistrationResponse> },
		};




uint32_t Types::FromBuffer(const Byte* Buffer, const size_t Capacity, TransferredData*& Transferable)
{
	if (!BaseHeader::MinimumCheck(Capacity))
		return false;

	auto FindIter = TypeHandlers.find(BaseHeader::BufferType(Buffer));
	if (FindIter == TypeHandlers.end())
		return false;

	TransferredData* Data = FindIter->second();

	uint32_t err = Data->FromBuffer(Buffer, Capacity);

	if (err) {
		Transferable = nullptr;
		delete Data;
	}
	else
		Transferable = Data;

	return err;
}
