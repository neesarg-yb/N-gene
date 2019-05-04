#pragma once
#include "NamedPropertyBase.hpp"

template <typename T>
class NamedPropertyType : public NamedPropertyBase
{
public:
	 NamedPropertyType( T const &value ) : m_value( value ) { }
	~NamedPropertyType() { }

protected:
	T m_value;

public:
	T GetValue() const;
};

template <typename T>
T NamedPropertyType<T>::GetValue() const
{
	return m_value;
}
