#pragma once
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/External/glcorearb.h"

class FrameBuffer
{
public:
	 FrameBuffer(); 
	~FrameBuffer();

	unsigned int GetHandle() const;
	unsigned int GetWidth () const;			// Width  of m_color_targer
	unsigned int GetHeight() const;			// Height of m_color_target

	// should just update members
	// finalize does the actual binding
	void SetColorTarget			( Texture *color_target ); 
	void SetDepthStencilTarget	( Texture *depth_target ); 

	// setups the the GL frame buffer - called before us. 
	// TODO: Make sure this only does work if the targets
	// have changed.
	bool Finalize();

public:
	GLuint		m_handle;
	Texture*	m_color_target					= nullptr; 
	Texture*	m_depth_stencil_target			= nullptr;
};