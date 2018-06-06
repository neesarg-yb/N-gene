#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


Sampler::Sampler( eSamplerType type )
	: m_sampler_handle( NULL )
{
	bool hasCreated = false;

	if( type == SAMPLER_LINEAR )
		hasCreated = CreateLinear();
	else
		hasCreated = CreateDefault();

	GUARANTEE_RECOVERABLE( hasCreated, "Sampler creation, FAILED!" );
}

Sampler::~Sampler()
{
	Destroy();
}

GLuint Sampler::GetHandle() const
{
	return m_sampler_handle;
}


bool Sampler::CreateDefault()
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
	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST ); // For 2D use GL_NEAREST
	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); // For 2D use GL_NEAREST
	return true; 
}

bool Sampler::CreateLinear()
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
	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // 3D & CubeMap use GL_LINEAR
	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); // 3D & CubeMap use GL_LINEAR
	return true; 
}

void Sampler::Destroy()
{
	if (m_sampler_handle != NULL) {
		glDeleteSamplers( 1, &m_sampler_handle ); 
		m_sampler_handle = NULL; 
	}
} 