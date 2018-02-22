#pragma once
#include <stdint.h>
#include <string>

typedef uint8_t Byte;
typedef std::wstring MessageContent;

using ID_t = uint32_t;

const ID_t INVALID_ID = 0;
const size_t PacketMaxSize = 512;

