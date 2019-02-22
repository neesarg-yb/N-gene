#pragma once
#include "ChunkFile.hpp"

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
	UNUSED( filePath );

	return false;
}

bool ChunkFile::ReadHeader( ChunkFileHeader &header_out )
{
	UNUSED( header_out );

	return false;
}

bool ChunkFile::GetNextBlocks( int &numBlocks_out, eBlockType &blockType_out )
{
	UNUSED( numBlocks_out );
	UNUSED( blockType_out );

	return false;
}
