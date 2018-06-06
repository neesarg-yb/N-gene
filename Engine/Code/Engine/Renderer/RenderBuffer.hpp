#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/External/glcorearb.h"
#include "Engine/Renderer/glfunctions.hpp"

class RenderBuffer
{
public:
	 RenderBuffer();   // initialize data
	~RenderBuffer();  // cleanup OpenGL resource 

					  // copies data to the GPU
	bool	CopyToGPU( size_t const byte_count, void const *data ); 
	GLuint	GetHandle();

public:
	GLuint handle;       // OpenGL handle to the GPU buffer, defualt = NULL; 
	size_t buffer_size;  // how many bytes are in this buffer, default = 0
};