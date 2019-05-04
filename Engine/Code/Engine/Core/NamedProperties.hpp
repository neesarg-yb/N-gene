#pragma once
#include <map>
#include <string>
#include "Engine/Core/NamedPropertyBase.hpp"
#include "Engine/Core/NamedPropertyType.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

typedef std::map< std::string, NamedPropertyBase* > NamedPropertiesMap;

class NamedProperties
{
public:
	 NamedProperties();
	~NamedProperties();

private:
	NamedPropertiesMap m_properties;

public:
	template <typename T>
		void Set( std::string const &name, T const &value );

	template <typename T>
		T Get( std::string const &name, T defaultValue ) const;

	// We treat "const char*" as "std::string"
	inline void			Set( std::string const &name, char const *value );
	inline std::string	Get( std::string const &name, char const *defaultValue );
};

template <typename T>
void NamedProperties::Set( std::string const &name, T const &value )
{
	NamedPropertiesMap::iterator it = m_properties.find( name );
	
	// If a value by this name already exists
	if( it != m_properties.end() )
	{
		bool typeMismatched = ( typeid( value ) != typeid( *it->second ) );
		if( typeMismatched )
			ERROR_RECOVERABLE( "NamedProperty's type is mimatched!" );

		// delete the value
		delete it->second;
		it->second = nullptr;
	}

	NamedPropertyBase *valuePropBase = new NamedPropertyType< T >( value );
	m_properties[ name ] = valuePropBase;
}

template <typename T>
T NamedProperties::Get( std::string const &name, T defaultValue ) const
{
	NamedPropertiesMap::const_iterator it = m_properties.find( name );

	if( it == m_properties.end() )
		return defaultValue;
	else
	{
		NamedPropertyType<T>* foundPropertyValue = (NamedPropertyType<T>*) it->second;
		return foundPropertyValue->GetValue();
	}
}

void NamedProperties::Set( std::string const &name, char const *value )
{
	Set( name, std::string( value ) );
}

std::string NamedProperties::Get( std::string const &name, char const *defaultValue )
{
	return Get( name, std::string( defaultValue ) );
}
