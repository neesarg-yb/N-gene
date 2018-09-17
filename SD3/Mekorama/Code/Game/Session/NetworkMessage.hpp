#pragma once
#include <string>

class NetworkMessage
{
public:
	 NetworkMessage( char const *msgName );
	~NetworkMessage();

public:
	bool Read( float &outFloat ) const;
	bool Read( std::string &outString ) const;

	void Write( float number );
	void Write( std::string &message );
};