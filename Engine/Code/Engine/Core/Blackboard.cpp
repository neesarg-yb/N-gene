#include "Blackboard.hpp"

Blackboard* g_gameConfigBlackboard = nullptr;

Blackboard::Blackboard()
{

}

Blackboard::~Blackboard()
{

}

void Blackboard::PopulateFromXmlElementAttributes( const XMLElement& element )
{
	// For every attributes of the element
	for( XMLAttribute* currentAttribute = (XMLAttribute*) element.FirstAttribute(); currentAttribute; currentAttribute = (XMLAttribute*) currentAttribute->Next() )
	{
		// Get name and value
		std::string attributeName  = std::string( currentAttribute->Name()  );
		std::string attributeValue = std::string( currentAttribute->Value() );
		
		// Add it to map
		this->SetValue( attributeName , attributeValue );
	}
}

void Blackboard::SetValue( const std::string& keyName, const std::string& newValue )
{
	m_keyValuePairs[keyName] = newValue;
}

bool Blackboard::GetValue( const std::string& keyName, bool defaultValue ) const
{
	bool returnValue = defaultValue;
	std::map< std::string , std::string >::const_iterator it = m_keyValuePairs.find( keyName );

	if( it != m_keyValuePairs.end() )
		SetFromText( returnValue , it->second.c_str() );

	return returnValue;
}

int	Blackboard::GetValue( const std::string& keyName, int defaultValue ) const
{
	int returnValue = defaultValue;
	std::map< std::string , std::string >::const_iterator it = m_keyValuePairs.find( keyName );

	if( it != m_keyValuePairs.end() )
		SetFromText( returnValue , it->second.c_str() );

	return returnValue;
}

float Blackboard::GetValue( const std::string& keyName, float defaultValue ) const
{
	float returnValue = defaultValue;
	std::map< std::string , std::string >::const_iterator it = m_keyValuePairs.find( keyName );

	if( it != m_keyValuePairs.end() )
		SetFromText( returnValue , it->second.c_str() );

	return returnValue;
}

std::string	Blackboard::GetValue( const std::string& keyName, std::string defaultValue ) const
{
	std::string returnValue = defaultValue;
	std::map< std::string , std::string >::const_iterator it = m_keyValuePairs.find( keyName );

	if( it != m_keyValuePairs.end() )
		returnValue = it->second;

	return returnValue;
}

std::string	Blackboard::GetValue( const std::string& keyName, const char* defaultValue ) const
{
	std::string returnValue = std::string( defaultValue );
	std::map< std::string , std::string >::const_iterator it = m_keyValuePairs.find( keyName );

	if( it != m_keyValuePairs.end() )
		returnValue = it->second;

	return returnValue;
}

Rgba Blackboard::GetValue( const std::string& keyName, const Rgba& defaultValue ) const
{
	Rgba returnValue = defaultValue;
	std::map< std::string , std::string >::const_iterator it = m_keyValuePairs.find( keyName );

	if( it != m_keyValuePairs.end() )
		returnValue.SetFromText( it->second.c_str() );

	return returnValue;
}

Vector2	Blackboard::GetValue( const std::string& keyName, const Vector2& defaultValue ) const
{
	Vector2 returnValue = defaultValue;
	std::map< std::string , std::string >::const_iterator it = m_keyValuePairs.find( keyName );

	if( it != m_keyValuePairs.end() )
		returnValue.SetFromText( it->second.c_str() );

	return returnValue;
}

IntVector2 Blackboard::GetValue( const std::string& keyName, const IntVector2& defaultValue ) const
{
	IntVector2 returnValue = defaultValue;
	std::map< std::string , std::string >::const_iterator it = m_keyValuePairs.find( keyName );

	if( it != m_keyValuePairs.end() )
		returnValue.SetFromText( it->second.c_str() );

	return returnValue;
}

FloatRange Blackboard::GetValue( const std::string& keyName, const FloatRange& defaultValue ) const
{
	FloatRange returnValue = defaultValue;
	std::map< std::string , std::string >::const_iterator it = m_keyValuePairs.find( keyName );

	if( it != m_keyValuePairs.end() )
		returnValue.SetFromText( it->second.c_str() );

	return returnValue;
}

IntRange Blackboard::GetValue( const std::string& keyName, const IntRange& defaultValue ) const
{
	IntRange returnValue = defaultValue;
	std::map< std::string , std::string >::const_iterator it = m_keyValuePairs.find( keyName );

	if( it != m_keyValuePairs.end() )
		returnValue.SetFromText( it->second.c_str() );

	return returnValue;
}