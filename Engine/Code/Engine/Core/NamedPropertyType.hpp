#pragma once
#include "NamedPropertyBase.hpp"

template <typename T>
class NamedPropertyType : public NamedPropertyBase
{
public:
	 NamedPropertyType( std::string const &name, T const &value );
	~NamedPropertyType() { }

protected:
	T m_value;

public:
	T GetValue() const;
};

template <typename T>
NamedPropertyType<T>::NamedPropertyType( std::string const &name, T const &value )
	: NamedPropertyBase( name )
	, m_value( value ) { }

template <typename T>
T NamedPropertyType<T>::GetValue() const
{
	return m_value;
}
