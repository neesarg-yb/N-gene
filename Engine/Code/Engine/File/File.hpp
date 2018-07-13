#pragma once
#include <stdio.h>
#include <fstream>
#include "Engine/Core/EngineCommon.hpp"

enum eFileOpenMode
{
	FILE_OPEN_MODE_READ = 0,
	FILE_OPEN_MODE_APPEND,
	FILE_OPEN_MODE_TRUNCATE,
	NUM_FILE_OPEN_MODES
};

class File
{
private:
	std::string		m_path = "";
	std::fstream	m_fileStream;

public:
	bool		Open( std::string const &filePath, eFileOpenMode mode );
	void		Close();

	bool inline	IsOpen() { return m_fileStream.is_open(); }

	void		Write( std::string const &dataString );
	void		Flush();
};

void*	FileReadToNewBuffer( char const *filename );
bool	FileWriteFromString( std::string const &fileName, std::string const &buffer );