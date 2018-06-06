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

unsigned int FrameBuffer::GetWidth( uint colorTargetIdx /* = 0 */ ) const
{
	unsigned int width = 0;

	if( m_color_targets[ colorTargetIdx ] != nullptr )
		width = m_color_targets[ colorTargetIdx ]->GetWidth();

	return width;
}

unsigned int FrameBuffer::GetHeight( uint colorTargetIdx /* = 0 */ ) const
{
	unsigned int height = 0;

	if( m_color_targets[ colorTargetIdx ] != nullptr )
		height = m_color_targets[ colorTargetIdx ]->GetHeight();

	return height;
}

void FrameBuffer::SetColorTarget( Texture *color_target, uint slot /* = 0 */ )
{
	m_color_targets[ slot ] = color_target;
}

void FrameBuffer::SetDepthStencilTarget( Texture *depth_target )
{
	m_depth_stencil_target = depth_target;
}

bool FrameBuffer::Finalize()
{
	GL_CHECK_ERROR();

	glBindFramebuffer( GL_FRAMEBUFFER, m_handle ); 

	// keep track of which outputs go to which attachments; 
	GLenum targets[ MAX_COLOR_TARGETS ]; 
 
	for( uint i = 0; i < MAX_COLOR_TARGETS; i++ )
	{
		if( m_color_targets[i] != nullptr )
		{
			// Bind color targets to an attachment point
			glFramebufferTexture( GL_FRAMEBUFFER, 
								  GL_COLOR_ATTACHMENT0 + i, 
								  m_color_targets[i]->GetHandle(), 
								  0 ); 

			// and keep track of which locations to which attachments
			targets[i] = GL_COLOR_ATTACHMENT0 + i;
		}
		else
		{
			targets[i] = GL_NONE;
		}
	}
	
	// Update target bindings
	glDrawBuffers( MAX_COLOR_TARGETS, targets ); 

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
	
	GL_CHECK_ERROR();

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