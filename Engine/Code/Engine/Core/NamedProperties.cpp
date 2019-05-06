#pragma once
#include "NamedProperties.hpp"

NamedProperties::NamedProperties()
{

}

NamedProperties::~NamedProperties()
{
	for( NamedPropertiesMap::iterator it = m_properties.begin(); it != m_properties.end(); it++ )
	{
		delete it->second;
		it->second = nullptr;
	}

	m_properties.clear();
}

