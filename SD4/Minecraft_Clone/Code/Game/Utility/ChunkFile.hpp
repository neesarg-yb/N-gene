#pragma once
#include <string>
#include "Game/GameCommon.hpp"
#include "Game/Utility/ChunkFileHeader.hpp"

class Block;

class ChunkFile
{
public:
	 ChunkFile();
	 ChunkFile( ChunkFileHeader const &headerToSave );
	~ChunkFile();

private:
	char	*m_buffer			= nullptr;
	int		 m_bufferSizeBytes	= 0;
	int		 m_readFrom			= 0;

	// Saving data
	ChunkFileHeader				m_headerToSave;
	std::vector< eBlockType >	m_blocksToSave;

public:
	// Read operation
	bool	Open( std::string const &filePath );
	bool	ReadHeader( ChunkFileHeader &header_out );
	bool	GetNextBlocks( int &numBlocks_out, eBlockType &blockType_out );

public:
	// Write operation
	void	AddNewBlock( Block const &block );
	void	SaveToFile( std::string const &filePath );
};

void* ReadBinaryFileToNewBuffer( char const *filePath, size_t &bufferSize_out );
