#pragma once
#include <queue>
#include "SpinLock.hpp"

template <typename T>
class ThreadSafeQueue
{
private:
	std::deque<T>	m_data; 
	SpinLock		m_lock; 

public:
	void Enqueue( T const &v )
	{
		m_lock.Enter();

		// I AM THE ONLY PERSON HERE
		m_data.push_back( v ); 

		m_lock.Leave();
		// no longer true...
	}


	// return if it succeeds
	bool Dequeue( T *out_v ) 
	{
		m_lock.Enter();

		bool hasData = !m_data.empty();
		if ( hasData ) 
		{
			*out_v = m_data.front();
			m_data.pop_front(); 
		}

		m_lock.Leave();
		return hasData; 
	}
};