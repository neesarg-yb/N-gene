#pragma once
#include "File.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdlib.h>
#include <filesystem>
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"

void* FileReadToNewBuffer( char const *filename )
{
	FILE *fp = nullptr;
	fopen_s( &fp, filename, "r" );

	if (fp == nullptr) {
		return nullptr;
	}

	size_t size = 0U;
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	char *buffer = (char*) malloc(size + 1);

	size_t read = fread( buffer, 1, size, fp );
	fclose(fp);

	buffer[read] = 0;

	return buffer; 
}

bool FileWriteFromString( std::string const &fileName, std::string const &buffer )
{
	std::ofstream fileWriter( fileName );

	if( fileWriter.is_open() == false )
		return false;

	fileWriter << buffer;
	fileWriter.close();

	return true;
}

bool File::Open( std::string const &filePath, eFileOpenMode mode )
{
	// If opened file is a different one
	if( this->IsOpen() && (m_path != filePath) )
		Close();

	// Set the path
	m_path = filePath;

	// To extract path from filename
	char path_buffer[ _MAX_PATH ];
	char drive[ _MAX_DRIVE ];
	char dir[ _MAX_DIR ];
	char filename[ _MAX_FNAME ];
	char ext[ _MAX_EXT ];
	errno_t err;

	err = strncpy_s(path_buffer, m_path.c_str(), _MAX_PATH - 1);
	GUARANTEE_RECOVERABLE( err == 0, Stringf("File::Open filePath's length seems grater than %i!", _MAX_PATH) );
	path_buffer[_MAX_PATH - 1] = '\0';

	err = _splitpath_s( path_buffer, drive, _MAX_DRIVE, dir, _MAX_DIR, filename, _MAX_FNAME, ext, _MAX_EXT );
	GUARANTEE_RECOVERABLE( err == 0, "File::Open error splitting the filePath!!" );

	std::string fileDirectory = Stringf( "%s%s", drive, dir );
	err = CreateDirectoryA( fileDirectory.c_str(), NULL );
	bool directoryIsInPlace = (err != 0) || (ERROR_ALREADY_EXISTS == GetLastError() );
	GUARANTEE_RECOVERABLE( directoryIsInPlace, Stringf("File::Open File directory %s isn't in place!!", fileDirectory.c_str()) );
	
	// try to open it
	switch ( mode )
	{
	case FILE_OPEN_MODE_READ:
		m_fileStream.open( m_path.c_str(), std::fstream::in );
		break;
	case FILE_OPEN_MODE_APPEND:
		m_fileStream.open( m_path.c_str(), std::fstream::out | std::fstream::app );
		break;
	case FILE_OPEN_MODE_TRUNCATE:
		m_fileStream.open( m_path.c_str(), std::fstream::out | std::fstream::trunc );
		break;
	default:
		GUARANTEE_OR_DIE( false, "File: invalid open mode passed!" );
		break;
	}

	return m_fileStream.is_open();
}

void File::Close()
{
	// Reset the path
	m_path = "";
	
	// Close file & delete the pointer
	m_fileStream.close();
}

void File::Write( std::string const &dataString )
{
	// Write with append mode
	m_fileStream << dataString;
}

void File::Flush()
{
	m_fileStream << std::flush;
}

bool File::ReadNextLine( std::string &out_line )
{
	if( std::getline( m_fileStream, out_line ) )
		return true;
	else
		return false;
}

bool File::Copy( std::string const &sourceFile, std::string const &destFile )
{
	return CopyFileA( sourceFile.c_str(), destFile.c_str(), false );
}
