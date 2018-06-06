#pragma once
#include "Engine/Renderer/External/glcorearb.h"
#include "Engine/Renderer/glfunctions.hpp"

class Sampler
{
public:
	 Sampler();
	~Sampler();

public:
	GLuint m_sampler_handle;

public:
	GLuint GetHandle();

private:
	bool create();
	void destroy();
};