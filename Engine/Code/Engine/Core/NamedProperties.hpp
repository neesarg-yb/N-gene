#pragma once
#include <map>
#include <string>
#include "Engine/Core/StringUtils.hpp"
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

	// We treat "char const*" as "std::string"
	inline void			Set( std::string const &name, char const *value );
	inline std::string	Get( std::string const &name, char const *defaultValue );
};

template <typename T>
void NamedProperties::Set( std::string const &name, T const &value )
{
	NamedPropertyBase *newValuePropBase	= new NamedPropertyType< T >( name, value );
	NamedPropertiesMap::iterator it		= m_properties.find( name );

	// If a value by this name already exists
	if( it != m_properties.end() )
	{
		// Check: new type should be same as old
		type_info const &newValueTypeInfo = typeid( *newValuePropBase );
		type_info const &oldValueTypeInfo = typeid( *it->second );

		// If not, this must be a typo by programmer
		bool typeMismatched = ( newValueTypeInfo != oldValueTypeInfo );
		if( typeMismatched )
			ERROR_RECOVERABLE( Stringf("Type Mismatched: \"%s\" is of %s. New value of %s should not be asigned!", name.c_str(), oldValueTypeInfo.name(), newValueTypeInfo.name() ) );

		// delete the value
		delete it->second;
		it->second = nullptr;
	}

	m_properties[ name ] = newValuePropBase;
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
