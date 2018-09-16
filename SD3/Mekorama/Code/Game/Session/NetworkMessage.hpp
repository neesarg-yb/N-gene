#pragma once
#include <string>

class NetworkMessage
{
public:
	 NetworkMessage( char const *msgName );
	~NetworkMessage();

public:
	bool Read( std::string &outString ) const;
	bool Read( float &outFloat ) const;
};