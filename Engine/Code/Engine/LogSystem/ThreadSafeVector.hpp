#pragma once
#include <vector>
#include "Engine/LogSystem/SpinLock.hpp"

template <typename T>
class ThreadSafeVector
{
private:
	std::vector<T>	m_vector;
	SpinLock		m_lock;

public:

	bool Contains( T const &v )
	{
		for( uint i = 0; i < m_vector.size(); i++ )
		{
			if( m_vector[i] == v )
				return true;
		}

		return false;
	}

	void Add( T const &v )
	{
		m_vector.push_back( v );
	}

	void Remove( T const &v )
	{
		for( uint i = 0; i < m_vector.size(); i++ )
		{
			if( m_vector[i] == v )
			{
				std::swap( m_vector[i], m_vector.back() );
				m_vector.pop_back();
				return;
			}
		}
	}

	void AddUnique( T const &v )	// Makes sure that there is just one copy of this value
	{
		if( Contains(v) )
			RemoveAll(v);

		Add(v);
	}

	void RemoveAll( T const &v )	// Removes all the entries having this value
	{
		// Remove every entries of v in the vector
		for( uint i = 0; i < m_vector.size(); i++ )
		{
			if( m_vector[i] == v )
			{
				std::swap( m_vector[i], m_vector.back() );
				m_vector.pop_back();
			}
		}
	}
};