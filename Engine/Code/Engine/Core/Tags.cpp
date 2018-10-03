#pragma once
#include "Tags.hpp"
#include "Engine/Math/MathUtil.hpp"

Tags::Tags()
{

}

void Tags::GetTags( Strings &outTags ) const
{
	outTags = m_tags;
}

void Tags::SetOrRemoveTags( const std::string& commaSeparatedTagNames )
{
	// Get vector of separated strings
	Strings separatedStrings;
	SetFromText( separatedStrings, ",", commaSeparatedTagNames.c_str() );

	for( std::string eachStr : separatedStrings )
	{
		// If "" is passed, skip it
		if( eachStr.length() <= 0 )
			continue;

		char startChar = eachStr.at( 0 );
		// If starts with !: remove operation
		if( startChar == '!' )
		{
			std::string tagToRemove( eachStr, 1, eachStr.length() - 1 );
			RemoveTag( tagToRemove );
		}
		// Else: Set operation
		else
		{
			SetTag( eachStr );
		}
	}
}

bool Tags::HasTags( const std::string& commaSeparatedTagNames )
{
	bool hasAllTags = true;

	// Get vector of separated strings
	Strings separatedStrings;
	SetFromText( separatedStrings, ",", commaSeparatedTagNames.c_str() );

	for( std::string eachStr : separatedStrings )
	{
		// If "" is passed, skip it
		if( eachStr.length() <= 0 )
			continue;

		// If starts with !
		char startChar = eachStr.at( 0 );
		if( startChar == '!' )
		{
			std::string tagToCheck( eachStr, 1, eachStr.length() - 1 );
			
			// it shouldn't have the tag
			if( HasTag(tagToCheck) == true )
				hasAllTags = false;
		}
		else
		{
			// it should have the tag
			if( HasTag(eachStr) == false )
				hasAllTags = false;
		}
	}

	return hasAllTags;
}

void Tags::SetTag( const std::string& tagName )
{
	if( HasTag(tagName) == false )
	{
		m_tags.push_back( tagName );
	}
}

void Tags::RemoveTag( const std::string& tagName )
{
	for( unsigned int i = 0; i < m_tags.size(); i++ )
	{
		if( m_tags[i] == tagName )
		{
			m_tags.erase( m_tags.begin() + i );
			break;
		}
	}
}

bool Tags::HasTag( const std::string& tagName )
{
	for( unsigned int i = 0; i < m_tags.size(); i++ )
	{
		if( m_tags[i] == tagName )
			return true;
	}

	return false;
}