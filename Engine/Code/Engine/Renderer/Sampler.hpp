#pragma once
#include "Engine/Renderer/External/glcorearb.h"
#include "Engine/Renderer/glfunctions.hpp"

enum eSamplerType
{
	SAMPLER_LINEAR,
	SAMPLER_NEAREST,
	SAMPLER_SHADOW,
	NUM_SAMPLER_TYPES
};

class Sampler
{
public:
	 Sampler( eSamplerType type = SAMPLER_NEAREST );
	~Sampler();

public:
	GLuint m_sampler_handle;

public:
	GLuint GetHandle() const;

private:
	bool CreateDefault();
	bool CreateLinear ();
	bool CreateShadow ();
	void Destroy();
};