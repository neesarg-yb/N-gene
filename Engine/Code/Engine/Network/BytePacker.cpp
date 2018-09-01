#pragma once
#include "BytePacker.hpp"

BytePacker::BytePacker( eEndianness byteOrder /* = LITTLE_ENDIAN */ )
	: m_endianness( byteOrder )
{
	// BytePacker owns buffer, I can grow it
	m_settings		= (BYTEPACKER_OWNS_MEMORY | BYTEPACKER_CAN_GROW);
	ReAllocateBufferOfSizeAndCopy( m_bufferSizeUnit );
}

BytePacker::BytePacker( size_t bufferSize, eEndianness byteOrder /* = LITTLE_ENDIAN */ )
	: m_endianness( byteOrder )
{
	// BytePacker owns buffer, I can't grow it
	m_settings = BYTEPACKER_OWNS_MEMORY;
	ReAllocateBufferOfSizeAndCopy( bufferSize );
}

BytePacker::BytePacker( size_t bufferSize, void *buffer, eEndianness byteOrder /* = LITTLE_ENDIAN */ )
	: m_endianness( byteOrder )
{
	// BytePacker don't own buffer, I can't grow it
	m_settings		= 0U;
	m_buffer		= buffer;
	m_bufferSize	= bufferSize;
}

BytePacker::~BytePacker()
{
	// If byte packer owns the memory, free it!
	if( (m_settings & BYTEPACKER_OWNS_MEMORY) == BYTEPACKER_OWNS_MEMORY )
	{
		free( m_buffer );
		m_buffer		= nullptr;
		m_bufferSize	= 0U;
	}
}

void BytePacker::SetEndianness( eEndianness endieanness )
{
	UNUSED( endieanness );
}

eEndianness BytePacker::GetEndianness() const
{
	return m_endianness;
}

bool BytePacker::WriteBytes( size_t byteCount, void const *data, bool changeEndiannessToSettings /* = true */ )
{
	size_t requiredMinimumBufferSize = m_writeHead + byteCount;

	// If buffer is smaller..
	if( requiredMinimumBufferSize > m_bufferSize )
	{
		// If can grow
		if( (m_settings & BYTEPACKER_CAN_GROW) == BYTEPACKER_CAN_GROW )
		{
			size_t newBufferSize = GetNewBufferSizeToStoreDataOfSize( requiredMinimumBufferSize );
			ReAllocateBufferOfSizeAndCopy( newBufferSize );
		}
		else
			return false;	// Can't grow the buffer, return
	}

	// Write new things in the buffer..
	byte_t	*byteBuffer			= (byte_t *)m_buffer;
	void	*writeNewDataFrom	= &byteBuffer[ m_writeHead ];

	// make a duplicate copy of data
	void *copiedData = malloc( byteCount );
	memcpy( copiedData, data, byteCount );

	// change endianness if required
	if( changeEndiannessToSettings )
		ChangeEndiannessTo( byteCount, copiedData, m_endianness );

	// write to buffer
	memcpy( writeNewDataFrom, copiedData, byteCount );
	m_writeHead += byteCount;

	// free duplicated data
	free( copiedData );

	return true;
}

size_t BytePacker::WriteSize( size_t size )
{
	size_t numBitsRequired	= GetTotalBitsRequiredToRepresent( size ); 
	size_t numBytesRequired	= (size_t) ceil( (double)numBitsRequired / 7.0 );

	// Check if buffer needs to grow to store new data..
	size_t minBufferSize = GetWrittenByteCount() + numBytesRequired;
	if( minBufferSize > m_bufferSize )
	{
		// If can't grow, return false
		if( (m_settings & BYTEPACKER_CAN_GROW) != BYTEPACKER_CAN_GROW )
			return 0U;
		else
		{
			// Grow the buffer
			size_t newBufferSize = GetNewBufferSizeToStoreDataOfSize( minBufferSize );
			ReAllocateBufferOfSizeAndCopy( newBufferSize );
		}
	}

	// until we write down all the bits
	for ( long int numBitsToWrite = (long int)numBitsRequired; numBitsToWrite >= 0; numBitsToWrite -= 7 )
	{
		byte_t	anEncodedByte	= 0U;

		// Write 7 bits
		for( int i = 0; i < 7; i++ )
		{
			byte_t bitToWrite = (size & 0b0000'0001) ? 0b1000'0000 : 0b0000'0000;
			size >>= 1;

			anEncodedByte >>= 1;
			anEncodedByte = anEncodedByte | bitToWrite;
		}
		
		// Write the the Continue Flag bit
		bool	numNotFinished	= ( numBitsToWrite - 7 ) > 0;
		byte_t	continueFlag	= ( numNotFinished ? 0b1000'0000 : 0b0000'0000 );		// notFinished = 1000 0000; finished = 0000 0000

		// Shift to left, and append the Continue Flag bit
		anEncodedByte >>= 1;
		anEncodedByte = anEncodedByte | continueFlag;
		
		bool written = WriteBytes( 1, &anEncodedByte, false );			// We don't flip order of encoded size
		GUARANTEE_RECOVERABLE( written == true, "BytePacker: Unexpected write failure!!" );
	}

	return numBytesRequired;
}

bool BytePacker::WriteString( char const *str )
{
	std::string stringToWrite	= str;

	return WriteBytes( stringToWrite.size(), stringToWrite.data() );
}

size_t BytePacker::ReadBytes( void *outData, size_t maxByteCount )
{
	size_t dataLengthInBuffer	= m_writeHead;
	size_t readableBytes		= (dataLengthInBuffer - m_readHead);
	size_t bytesToRead			= (readableBytes < maxByteCount) ? readableBytes : maxByteCount;

	byte_t	*byteBuffer			= (byte_t *)m_buffer;
	void	*readFromPointer	= &byteBuffer[ m_readHead ];
	memcpy( outData, readFromPointer, bytesToRead );

	return bytesToRead;
}

size_t BytePacker::ReadSize( size_t *outSize )
{
	UNUSED( outSize );

	return 0U;
}

size_t BytePacker::ReadString( char *outStr, size_t maxByteSize )
{
	size_t bytesGotRead			= ReadBytes( outStr, maxByteSize );
	outStr[ bytesGotRead - 1 ]	= '\0';								// Since it is a string, end it with null terminator

	return bytesGotRead;
}

bool BytePacker::SetWrittenByteCountDummy( size_t byteCount )
{
	// I don't see use case for this function, yet..
	UNUSED( byteCount );

	return false;
}

void BytePacker::ResetWrite()
{
	// Empty the buffer; resetting its size, as well.
	free( m_buffer );
	m_bufferSize	= m_bufferSizeUnit;
	m_buffer		= malloc( m_bufferSize );

	// Reset read & write heads
	m_writeHead	= 0U;
	m_readHead	= 0U;
}

void BytePacker::ResetRead()
{
	m_readHead = 0U;
}

size_t BytePacker::GetWrittenByteCount() const
{
	return m_writeHead;
}

size_t BytePacker::GetWritableByteCount() const
{
	return (m_bufferSize - m_writeHead);
}

size_t BytePacker::GetReadableByteCount() const
{
	return m_writeHead;
}

void BytePacker::ReAllocateBufferOfSizeAndCopy( size_t newBufferSize )
{
	
	// Create a new buffer
	void *newBuffer = malloc( newBufferSize );

	// Copy content over, if there is any..
	size_t copyContentSize = (m_writeHead < newBufferSize) ? m_writeHead : newBufferSize;		// If new buffer size if smaller, copy only the amount of data it can store
	memcpy( newBuffer, m_buffer, copyContentSize );

	// Delete the content of old m_buffer, if there are any..
	if( m_buffer != nullptr )
	{
		free( m_buffer );
		m_buffer		= nullptr;
		m_bufferSize	= 0U;
	}

	// Re-point the m_buffer to the new one.
	m_buffer		= newBuffer;
	m_bufferSize	= newBufferSize;
	m_writeHead		= copyContentSize;
	m_readHead		= 0U;
}

size_t BytePacker::GetNewBufferSizeToStoreDataOfSize( size_t minimumRequiredBufferSize )
{
	// Pattern: 8kb * n => 8, 16, 32, 64, ..
	//			=> bufferSizeUnit * n
	size_t n		= (size_t)ceil( (double)minimumRequiredBufferSize / m_bufferSizeUnit );

	return m_bufferSizeUnit * n;
}

size_t BytePacker::GetTotalBitsRequiredToRepresent( size_t number )
{
	size_t bitsRequired = (size_t) log2l( (long double)number ) + 1U;
	return bitsRequired;
}

