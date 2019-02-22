#pragma once
#include "ChunkFile.hpp"
#include "Game/World/Block.hpp"

ChunkFile::ChunkFile()
{

}

ChunkFile::ChunkFile( ChunkFileHeader const &headerToSave )
	: m_headerToSave( headerToSave )
{

}

ChunkFile::~ChunkFile()
{
	if( m_buffer != nullptr )
	{
		delete m_buffer;
		m_buffer = nullptr;
	}
}

bool ChunkFile::Open( std::string const &filePath )
{
	if( m_buffer != nullptr )
	{
		delete m_buffer;
		m_buffer = nullptr;

		m_bufferSizeBytes = 0;
		m_readFrom = 0;
	}

	size_t bufferBytesCount = 0U;
	m_buffer			= (char*) ReadBinaryFileToNewBuffer( filePath.c_str(), bufferBytesCount );
	m_bufferSizeBytes	= (int) bufferBytesCount;

	return (m_buffer != nullptr);
}

bool ChunkFile::ReadHeader( ChunkFileHeader &header_out )
{
	int const headerBytesCount = 12;
	if( m_readFrom + headerBytesCount > m_bufferSizeBytes )
		return false;

	// Fetch 4CC
	for( int ccIdx = 0; ccIdx < 4; ccIdx++ )
	{
		header_out.m_4cc[ ccIdx ] = m_buffer[ m_readFrom++ ];
	}

	// 4CC doesn't match
	if( header_out.GetString4CC() != "SMCD" )
		return false;

	// Version
	header_out.m_version	= m_buffer[ m_readFrom++ ];
	
	// Chunk size data
	header_out.m_chunkBitsX	= m_buffer[ m_readFrom++ ];
	header_out.m_chunkBitsY	= m_buffer[ m_readFrom++ ];
	header_out.m_chunkBitsZ	= m_buffer[ m_readFrom++ ];

	// Reserved bytes
	header_out.m_reserved1	= m_buffer[ m_readFrom++ ];
	header_out.m_reserved2	= m_buffer[ m_readFrom++ ];
	header_out.m_reserved3	= m_buffer[ m_readFrom++ ];

	// Encoding Format
	header_out.m_format		= m_buffer[ m_readFrom++ ];

	return true;
}

bool ChunkFile::GetNextBlocks( int &numBlocks_out, eBlockType &blockType_out )
{
	if( m_readFrom + 2 > m_bufferSizeBytes )
		return false;

	uchar runType  = 0; 
	uchar runCount = 0;

	runType  = m_buffer[ m_readFrom++ ];
	runCount = m_buffer[ m_readFrom++ ];

	numBlocks_out = (int) runCount;
	blockType_out = GetBlockTypeFromInteger( (int)runType );

	return true;
}

void ChunkFile::AddNewBlock( Block const &block )
{
	m_blocksToSave.push_back( block.GetType() );
}

void ChunkFile::SaveToFile( std::string const &filePath )
{
	std::vector< uchar > outByteVector;

	// Store the file header
	outByteVector.push_back( m_headerToSave.m_4cc[0] );
	outByteVector.push_back( m_headerToSave.m_4cc[1] );
	outByteVector.push_back( m_headerToSave.m_4cc[2] );
	outByteVector.push_back( m_headerToSave.m_4cc[3] );

	outByteVector.push_back( m_headerToSave.m_version );

	outByteVector.push_back( m_headerToSave.m_chunkBitsX );
	outByteVector.push_back( m_headerToSave.m_chunkBitsY );
	outByteVector.push_back( m_headerToSave.m_chunkBitsZ );

	outByteVector.push_back( m_headerToSave.m_reserved1 );
	outByteVector.push_back( m_headerToSave.m_reserved2 );
	outByteVector.push_back( m_headerToSave.m_reserved3 );

	outByteVector.push_back( m_headerToSave.m_format );

	// Store the blocks using RLE
	int blocksCompressed		= 1;
	int			runBlockCount	= 1;
	eBlockType	runBlockType	= m_blocksToSave[0];
	do
	{
		eBlockType &thisBlockType = m_blocksToSave[ blocksCompressed ];

		// This block type is different than what we're caching
		if( thisBlockType != runBlockType )
		{
			// Encode the previous pending blocks
			outByteVector.push_back( (uchar) runBlockType );
			outByteVector.push_back( (uchar) runBlockCount );

			// Start new batch
			runBlockType = thisBlockType;
			runBlockCount = 0;
		}

		// It is the same run block type!
		// If reached to the max count that can be represented by a uchar
		if( runBlockCount >= 255 )
		{
			// Encode the previous one batch
			outByteVector.push_back( (uchar) runBlockType );
			outByteVector.push_back( (uchar) runBlockCount );

			// Start new batch
			runBlockType = thisBlockType;
			runBlockCount = 0;
		}
		
		runBlockCount++;
		blocksCompressed++;

	} while( blocksCompressed < m_blocksToSave.size() );

	// Push the last encoded batch
	outByteVector.push_back( (uchar) runBlockType );
	outByteVector.push_back( (uchar) runBlockCount );


	// Write to the file!
	FILE *fp = nullptr;
	fopen_s( &fp, filePath.c_str(), "wb" );

	if (fp == nullptr)
		return;

	fwrite( outByteVector.data(), 1U, outByteVector.size(), fp );
	fclose( fp );
}

void* ReadBinaryFileToNewBuffer( char const *filePath, size_t &bufferSize_out )
{
	FILE *fp = nullptr;
	fopen_s( &fp, filePath, "rb" );

	if (fp == nullptr) 
	{
		return nullptr;
	}

	size_t size = 0U;
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	char *buffer = (char*) malloc(size);
	size_t read = fread( buffer, 1, size, fp );
	bufferSize_out = read;
	fclose(fp);

	return buffer; 
}
