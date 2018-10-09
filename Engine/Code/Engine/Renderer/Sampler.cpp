#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba.hpp"

Sampler::Sampler( eSamplerType type )
	: m_sampler_handle( NULL )
{
	bool hasCreated = false;

	if ( type == SAMPLER_LINEAR )
		hasCreated = CreateLinear();
	else if ( type == SAMPLER_SHADOW )
		hasCreated = CreateShadow();
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

	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );	// Default: GL_LINEAR
	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST );					// Default: GL_LINEAR

	glSamplerParameterf( m_sampler_handle, GL_TEXTURE_MIN_LOD, m_minMipMap ); 
	glSamplerParameterf( m_sampler_handle, GL_TEXTURE_MAX_LOD, m_maxMipMap ); 

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

	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );	// Default: GL_LINEAR
	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR );					// Default: GL_LINEAR

	glSamplerParameterf( m_sampler_handle, GL_TEXTURE_MIN_LOD, m_minMipMap ); 
	glSamplerParameterf( m_sampler_handle, GL_TEXTURE_MAX_LOD, m_maxMipMap ); 

	return true; 
}

bool Sampler::CreateShadow()
{
	// create the sampler handle if needed; 
	if (m_sampler_handle == NULL) {
		glGenSamplers( 1, &m_sampler_handle ); 
		if (m_sampler_handle == NULL) {
			return false; 
		}
	}

	// setup wrapping
	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );               
	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );               
	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER );               

	float color[4];
	RGBA_WHITE_COLOR.GetAsFloats( color[0], color[1], color[2], color[3] );
	glSamplerParameterfv( m_sampler_handle, GL_TEXTURE_BORDER_COLOR, color ); 

	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE ); 
	glSamplerParameteri( m_sampler_handle, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL ); 

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