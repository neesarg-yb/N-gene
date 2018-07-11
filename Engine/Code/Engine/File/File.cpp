#pragma once
#include "File.hpp"

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
	
	// try to open it
	switch ( mode )
	{
	case FILE_OPEN_MODE_READ:
		m_fileStream.open( m_path.c_str(), std::fstream::in );
		break;
	case FILE_OPEN_MODE_APPEND:
		m_fileStream.open( m_path.c_str(), std::fstream::app );
		break;
	case FILE_OPEN_MODE_TRUNCATE:
		m_fileStream.open( m_path.c_str(), std::fstream::trunc );
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