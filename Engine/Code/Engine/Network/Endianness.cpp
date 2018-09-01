#include "Endianness.hpp"

eEndianness GetPlatformEndianness()
{
	uint		numOne	= 1U;
	char const *array	= (char const *) &numOne;

	if( array[0] == 0x01 )
		return LITTLE_ENDIAN;
	else
		return BIG_ENDIAN;
}

void ChangeEndiannessTo( size_t const size, void *data, eEndianness endianness )
{
	// If it is same as your platform's endianness, return (assuming that buffer is from your machine)
	if( GetPlatformEndianness() == endianness )
		return;

	byte_t *byte_buffer = (byte_t*) data;

	uint i = 0;
	uint j = (uint)size - 1U;
	while( i < j )
	{
		std::swap( byte_buffer[i], byte_buffer[j] );	// you're swapping the byte order..
		++i;
		--j;
	}
}
