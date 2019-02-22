#pragma once
#include <string>
#include "Game/GameCommon.hpp"

struct ChunkFileHeader;

class ChunkFile
{
public:
	 ChunkFile();
	~ChunkFile();

private:
	char	*m_buffer	= nullptr;
	int		 m_readFrom	= 0;

public:
	bool	Open( std::string const &filePath );
	bool	ReadHeader( ChunkFileHeader &header_out );
	bool	GetNextBlocks( int &numBlocks_out, eBlockType &blockType_out );
};
