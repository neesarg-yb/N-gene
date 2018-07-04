#pragma once
#include "Engine\Renderer\External\glcorearb.h"

#define PROFILE_LOG_SCOPE(tag) ProfileLogScoped __timer__ ##__LINE__ ## (tag)
#define PROFILE_LOG_SCOPE_FUNCTION() ProfileLogScoped __timer__ ##__LINE__ ## (__FUNCTION__)

class ProfileLogScoped
{
public:
	 ProfileLogScoped( char const *tag );
	~ProfileLogScoped();

public:
	uint64_t	m_hpcStart;
	char const *m_tag;

private:
	uint64_t	GetPerformanceCounter() const;
	double		GetSecondsFromPerformanceCounter( uint64_t hpc ) const;
	double		GetMillliSecondsFromPerformanceCounter( uint64_t hpc ) const;
};