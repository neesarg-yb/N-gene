#pragma once
#include "Engine\Renderer\External\glcorearb.h"

class ProfileLogScoped
{
public:
	 ProfileLogScoped( char const *tag );
	~ProfileLogScoped();

public:
	uint64_t	m_hpcStart;
	char const *m_tag;
};