#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Network/Endianness.hpp"

#define BIT_FLAG(f)	(1U << (f))

enum eBytePackerOptionBit : uint
{
	// a byte packer allocated its own memory and 
	// should free it when destroyed; 
	BYTEPACKER_OWNS_MEMORY = BIT_FLAG(0),

	// If the byte packer runs out of space, will
	// allocate more memory (maintaining the data that was stored)
	// Must have flag BYTEPACKER_OWNS_MEMORY
	BYTEPACKER_CAN_GROW = BIT_FLAG(1)
};
typedef uint eBytePackerOptions; 

class BytePacker 
{
public:
	 BytePacker( eEndianness byteOrder = LITTLE_ENDIAN );									// BytePacker owns buffer, I can grow it
	 BytePacker( size_t bufferSize, eEndianness byteOrder = LITTLE_ENDIAN );				// BytePacker owns buffer, I can't grow it
	 BytePacker( size_t bufferSize, void *buffer, eEndianness byteOrder = LITTLE_ENDIAN );	// BytePacker don't own buffer, I can't grow it
	~BytePacker(); 

private:
	eEndianness			 m_endianness	= LITTLE_ENDIAN;
	eBytePackerOptions	 m_settings		= 0U;							// Doesn't owns the memory, and can't grow the buffer

protected:
	void				*m_buffer		= nullptr;
	size_t				 m_bufferSize	= 0U;							// Max bytes this buffer can store
	size_t mutable		 m_readHead		= 0U;
	size_t				 m_writeHead	= 0U;							// You can get total readable data size from this variable, as well.

	size_t const		 m_bufferSizeUnit = 8U * 1024U;					// Used by GetNewBufferSizeToStoreDataOfSize() & Reset()

public:
	// Properties
	void		SetEndianness( eEndianness endieanness );				// Changes the Endianness of the all the data written in buffer, and change the m_endianness
	eEndianness	GetEndianness() const;
	
	// Write in buffer
	bool		WriteBytes	( size_t byteCount, void const *data, bool changeEndiannessToSettings = true );		// Returns false if buffer can't hold the new data in it.. It doesn't write in buffer on failed attempt.
	size_t		WriteSize	( size_t size );							// Writes a size_t in buffer in a compresses way, returns how many bytes it used to represent passed size_t variable.
	bool		WriteString	( char const *str );						// Returns false if the string is too long to hold by buffer.. It doesn't write anything, in that case.

	// Read from the buffer
	size_t				ReadBytes	( void *outData, size_t maxByteCount, bool changeEndiannessToMachine = true ) const;		// Returns how many actual bytes got read
	size_t				ReadSize	( size_t *outSize ) const;					// Returns how many bytes got read to fetch the size_t, fills outSize
	size_t				ReadString	( char *outStr, size_t maxByteSize ) const;	// Note: maxByteSize should be enough to contain the null terminator as well
	inline void const*	GetBuffer() const { return m_buffer; }					// Access buffer without changing readHead!

	// Set buffer variables
	bool		SetWrittenByteCountDummy( size_t byteCount );			// Changes the writeHead, DO NOT USE TO MOVE IT BACKWORDS unless you're resetting it
	void		ResetWrite();											// Resets witeHead & readHead
	void		ResetRead() const;										// Resets just readHead

	// Get buffer variables
	size_t		GetWrittenByteCount() const;							// How much have I written to this buffer?
	size_t		GetWritableByteCount() const;							// How much more can I write to this buffer (if grow-able, this returns UINFINITY)
	size_t		GetReadableByteCount() const;							// How much more data can I read?

private:
	void		ReAllocateBufferOfSizeAndCopy		( size_t newBufferSize );
	size_t		GetNewBufferSizeToStoreDataOfSize	( size_t minimumRequiredBufferSize ) const;		// Gives size of new buffer in stages: 8kb, 16kb, 32kb, 64kb, 128kb, etc..
	size_t		GetTotalBitsRequiredToRepresent		( size_t number ) const;						// Doesn't count the continue flag of the BytePacker..
	size_t		GetTotalBytesRequiredToWriteSize	( size_t number ) const;						// Does account for continue bits in each byte
};
