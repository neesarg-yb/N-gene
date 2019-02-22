#pragma once
#include "ChunkFile.hpp"
#include "Game/Utility/ChunkFileHeader.hpp"

ChunkFile::ChunkFile()
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
