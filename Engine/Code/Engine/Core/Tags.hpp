#pragma once
#include <string>
#include <vector>

typedef std::vector< std::string > Strings;

class Tags
{
public:
	 Tags();

private:
	Strings m_tags;

public:
	void GetTags( Strings &outTags ) const;

public:
	 void SetOrRemoveTags( const std::string& commaSeparatedTagNames );		// "blessed,!poisoned"
	 bool HasTags( const std::string& commaSeparatedTagNames );				// "good,!cursed"

protected:
	void SetTag( const std::string& tagName );
	void RemoveTag( const std::string& tagName );
	bool HasTag( const std::string& tagName );
};