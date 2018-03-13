#pragma once
#include <stdint.h>
#include <string>
#include <algorithm>

typedef uint8_t Byte;
typedef std::wstring MessageContent;

using ID_t = uint32_t;

const ID_t INVALID_ID = 0;
const size_t PacketMaxSize = 512;

namespace Storing
{
	const unsigned LoginSizeMax = 250;
	// removed soon
	const unsigned PasswordSizeMax = 250;

	const unsigned MessageSizeMax = 500;
}

// for std container
template<typename _Std_Container>
size_t BytesContain(const _Std_Container& container)
{
	return container.size() * sizeof(*(container.begin()));
}


//IS allowed login
