#pragma once
#include "Engine/Core/EngineCommon.hpp"

enum eEndianness
{
	LITTLE_ENDIAN = 0,
	BIG_ENDIAN
};

eEndianness GetPlatformEndianness();

// Assumes that the data is in platform endianness, and will convert to passed endianness..
void ChangeEndiannessTo( size_t const size, void *data, eEndianness targetEndianness );

// Assumes that the data is in provided endianness, and will convert to platform's endianness..
void ChangeEndiannessFrom( size_t const size, void *data, eEndianness sourceEndianness );