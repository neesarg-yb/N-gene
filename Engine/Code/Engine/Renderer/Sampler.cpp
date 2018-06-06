#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


Sampler::Sampler()
	: m_sampler_handle( NULL )
{
	bool hasCreated = create();

	GUARANTEE_RECOVERABLE( hasCreated, "Sampler creation, FAILED!" );
}

Sampler::~Sampler()
{
	destroy();
}

GLuint Sampler::GetHandle()
{
	return m_sampler_handle;
}


bool Sampler::create()
{
	// create the sampler handle if needed; 
	if (m_sampler_handle == NULL) {
		glGenSamplers( 1, &m_sampler_handle ); 
		if (m_sampler_handle == NULL) {
			return false; 
		}
	}

	// setup wrapping
	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_WRAP_S, GL_REPEAT );  
	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_WRAP_T, GL_REPEAT );  
	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_WRAP_R, GL_REPEAT );  

	// filtering; 
	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	return true; 
}

void Sampler::destroy()
{
	if (m_sampler_handle != NULL) {
		glDeleteSamplers( 1, &m_sampler_handle ); 
		m_sampler_handle = NULL; 
	}
} 