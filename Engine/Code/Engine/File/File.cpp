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