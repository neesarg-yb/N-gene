#pragma once
#include <mutex>

class SpinLock
{
private:
	std::mutex m_lock; 

public:
	void inline Enter()		{ m_lock.lock(); }				// blocking operation
	bool inline TryEnter()	{ return m_lock.try_lock(); }
	void inline Leave()		{ m_lock.unlock(); }			// must be called if entered
};