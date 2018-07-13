#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>


//-----------------------------------------------------------------------------------------------
const std::string Stringv( const char* format, va_list variableArgumentList );
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );




