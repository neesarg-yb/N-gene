#pragma once
#include "Engine/Core/EngineCommon.hpp"

enum eEndianness
{
	LITTLE_ENDIAN = 0,
	BIG_ENDIAN
};

eEndianness GetPlatformEndianness();

// Assumes that the data is in platform endianness, and will convert to supported endianness..
void ChangeEndiannessTo( size_t const size, void *data, eEndianness endiannessOfData );
