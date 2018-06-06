#pragma once
#include "FrameBuffer.hpp"
#include "Engine/Renderer/glfunctions.hpp"

FrameBuffer::FrameBuffer()
{
	glGenFramebuffers( 1, &m_handle ); 
}

FrameBuffer::~FrameBuffer()
{
	if (m_handle != NULL) {
		glDeleteFramebuffers( 1, &m_handle ); 
		m_handle = NULL; 
	}
}

unsigned int FrameBuffer::GetHandle() const 
{
	return m_handle;
}

unsigned int FrameBuffer::GetWidth() const
{
	unsigned int width = 0;

	if( m_color_target != nullptr )
		width = m_color_target->GetWidth();

	return width;
}

unsigned int FrameBuffer::GetHeight() const
{
	unsigned int height = 0;

	if( m_color_target != nullptr )
		height = m_color_target->GetHeight();

	return height;
}

void FrameBuffer::SetColorTarget( Texture *color_target )
{
	m_color_target			= color_target;
}

void FrameBuffer::SetDepthStencilTarget( Texture *depth_target )
{
	m_depth_stencil_target			= depth_target;
}

bool FrameBuffer::Finalize()
{
	glBindFramebuffer( GL_FRAMEBUFFER, m_handle ); 

	// keep track of which outputs go to which attachments; 
	GLenum targets[1]; 

	// Bind a color target to an attachment point
	// and keep track of which locations to to which attachments. 
	glFramebufferTexture( GL_FRAMEBUFFER, 
		GL_COLOR_ATTACHMENT0 + 0, 
		m_color_target->GetHandle(), 
		0 ); 
	// 0 to to attachment 0
	targets[0] = GL_COLOR_ATTACHMENT0 + 0; 

	// Update target bindings
	glDrawBuffers( 1, targets ); 

	// Bind depth if available;
	if (m_depth_stencil_target == nullptr) {
		glFramebufferTexture( GL_FRAMEBUFFER, 
			GL_DEPTH_STENCIL_ATTACHMENT, 
			NULL, 
			0 ); 
	} else {
		glFramebufferTexture( GL_FRAMEBUFFER, 
			GL_DEPTH_STENCIL_ATTACHMENT, 
			m_depth_stencil_target->GetHandle(), 
			0 ); 
	}

	// Error Check - recommend only doing in debug
#if defined(_DEBUG)
	GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		DebuggerPrintf( "Failed to create framebuffer:  %u", status );
		return false;
	}
#endif

	return true;
}