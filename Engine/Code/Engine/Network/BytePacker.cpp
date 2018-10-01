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

BytePacker::BytePacker( BytePacker const &src )
{
	// copy all the properties
	m_endianness		= src.m_endianness;
	m_settings			= src.m_settings;
	m_bufferSize		= src.m_bufferSize;
	m_readHead			= src.m_readHead;
	m_writeHead			= src.m_writeHead;

	// memcpy buffer
	m_buffer = malloc( m_bufferSize );
	memcpy( m_buffer, src.GetBuffer(), m_bufferSize );
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
	m_endianness = endieanness;
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
	size_t numBitsRequired	= GetTotalBitsRequiredToRepresent( size );		// DOesn't include continue bit
	size_t numBytesRequired	= GetTotalBytesRequiredToWriteSize( size );		// Does account for continue bit

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
	size_t		stringLength	= stringToWrite.length();

	// If buffer is smaller..
	size_t requiredBytesForStrLength	= GetTotalBytesRequiredToWriteSize( stringLength );
	size_t requiredMinBufferSize		= GetWrittenByteCount() + stringLength + requiredBytesForStrLength;
	if( requiredMinBufferSize > m_bufferSize )
	{
		// If can grow
		if( (m_settings & BYTEPACKER_CAN_GROW) == BYTEPACKER_CAN_GROW )
		{
			size_t newBufferSize = GetNewBufferSizeToStoreDataOfSize( requiredMinBufferSize );
			ReAllocateBufferOfSizeAndCopy( newBufferSize );
		}
		else
			return false;	// Can't grow the buffer, return
	}

	size_t bytesWritten	= WriteSize( stringLength );
	GUARANTEE_RECOVERABLE( bytesWritten == requiredBytesForStrLength, "BytePacker: Unexpected error while writing size of string!" );

	bool writtenString	= WriteBytes( stringLength, str, false );
	GUARANTEE_RECOVERABLE( writtenString, "BytePacker: Unexpected write error!" );

	return true;
}

size_t BytePacker::ReadBytes( void *outData, size_t maxByteCount, bool changeEndiannessToMachine /* = true */ ) const
{
	size_t dataLengthInBuffer	= m_writeHead;
	size_t readableBytes		= (dataLengthInBuffer - m_readHead);
	size_t bytesToRead			= (readableBytes < maxByteCount) ? readableBytes : maxByteCount;

	byte_t	*byteBuffer			= (byte_t *)m_buffer;
	void	*readFromPointer	= &byteBuffer[ m_readHead ];
	memcpy( outData, readFromPointer, bytesToRead );
	
	// Progress the readHead
	m_readHead += bytesToRead;

	// Change Endianness
	if( changeEndiannessToMachine )
		ChangeEndiannessFrom( bytesToRead, outData, m_endianness );

	return bytesToRead;
}

size_t BytePacker::ReadSize( size_t *outSize ) const
{
	*outSize = 0U;

	size_t	totalReadBytes	= 0U;
	bool	continueReading = true;
	while ( continueReading )
	{
		// Read one byte
		byte_t nextByte		= 0x00;
		size_t nextByteSize	= ReadBytes( &nextByte, 1U, false );
		
		// If we can't read next byte, there is not anything more to read..
		if( nextByteSize != 1U )
			return 0U;				// Consider it as failed read
		
		// Read 7 bits
		byte_t fetchedByte			= ( nextByte & 0b0111'1111 );
		fetchedByte <<= 1;			// Because first bit is not part of number

		size_t numberMask			= 0U;
		byte_t *numberMaskBytePtr	= (byte_t *)&numberMask;
		numberMaskBytePtr[7]		= fetchedByte;
		
		// Write it to outSize
		*outSize >>= 7;
		*outSize  |= numberMask;

		// Check & continue
		continueReading = ( (nextByte & 0b1000'0000) == 0b1000'0000 );

		totalReadBytes += 1;
	}

	size_t needToShiftRightForBytes = (sizeof( size_t ) * 8) - (totalReadBytes * 7) ;
	*outSize >>= needToShiftRightForBytes;

	return totalReadBytes;
}

size_t BytePacker::ReadString( char *outStr, size_t maxByteSize ) const
{
	size_t stringLength	= 0U;
	size_t bytesRead	= ReadSize( &stringLength );
	
	// If failed to read a size..
	if( bytesRead == 0U )
	{
		outStr[0] = '\0';		// out a valid empty string!
		return 0U;
	}

	// Read full string - locally
	char	*fullString		= (char *) malloc( stringLength + 1 );
	size_t	 bytesGotRead	= ReadBytes( fullString, stringLength, false );
	GUARANTEE_RECOVERABLE( bytesGotRead == stringLength, "BytePacker ReadString couldn't read the whole string!" );

	// memcpy up to maxByteSize
	size_t bytesToMemcpy			= ( maxByteSize < (stringLength + 1) ) ? maxByteSize : (stringLength + 1);
	fullString[ bytesToMemcpy - 1 ] = '\0';
	memcpy( outStr, fullString, bytesToMemcpy );

	// Free the read string..
	free( fullString );

	return bytesGotRead;
}

bool BytePacker::MoveReadheadBy( double bytes ) const
{
	double newReadHead = (double)m_readHead + bytes;

	// If less than ZERO or more than the bytes written in buffer
	if( newReadHead < 0.f || newReadHead > m_writeHead )
		return false;

	m_readHead = (size_t)newReadHead;
	return true;
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

void BytePacker::ResetRead() const
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
	m_readHead		= ( m_readHead < newBufferSize ) ? m_readHead : 0U;
}

size_t BytePacker::GetNewBufferSizeToStoreDataOfSize( size_t minimumRequiredBufferSize ) const
{
	// Pattern: 8kb * n => 8, 16, 32, 64, ..
	//			=> bufferSizeUnit * n
	size_t n		= (size_t)ceil( (double)minimumRequiredBufferSize / m_bufferSizeUnit );

	return m_bufferSizeUnit * n;
}

size_t BytePacker::GetTotalBitsRequiredToRepresent( size_t number ) const
{
	size_t bitsRequired = (size_t) log2l( (long double)number ) + 1U;
	return bitsRequired;
}

size_t BytePacker::GetTotalBytesRequiredToWriteSize( size_t number ) const
{
	size_t numBitsRequired	= GetTotalBitsRequiredToRepresent( number ); 
	size_t numBytesRequired	= (size_t) ceil( (double)numBitsRequired / 7.0 );

	return numBytesRequired;
}
