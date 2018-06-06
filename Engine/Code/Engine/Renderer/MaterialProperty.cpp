#pragma once
#include "Engine/Renderer/MaterialProperty.hpp"
#include "Engine/Renderer/glfunctions.hpp"

MaterialPropertyFloat::MaterialPropertyFloat( char const *name, float data )
{
	m_name = name;
	m_data = data;
}

void MaterialPropertyFloat::Bind( unsigned int programHandle )
{
	GLint idx = glGetUniformLocation( programHandle, m_name.c_str() );
	if( idx  >= 0 )
		glUniform1fv( idx, 1, (GLfloat const *) &m_data );
}

void MaterialPropertyFloat::SetData( void const *data )
{
	m_data = *(float const *)data;
}

size_t MaterialPropertyFloat::GetByteSize() const
{
	return sizeof( m_data );
}

MaterialPropertyVector2::MaterialPropertyVector2( char const *name, Vector2 const &data )
{
	m_name = name;
	m_data = data;
}

void MaterialPropertyVector2::Bind( unsigned int programHandle )
{
	GLint idx = glGetUniformLocation( programHandle, m_name.c_str() );
	if( idx  >= 0 )
		glUniform2fv( idx, 1, (GLfloat const *) &m_data );
}

void MaterialPropertyVector2::SetData( void const *data )
{
	m_data = *(Vector2 const *)data;
}

size_t MaterialPropertyVector2::GetByteSize() const
{
	return sizeof( m_data );
}

MaterialPropertyVector3::MaterialPropertyVector3( char const *name, Vector3 const &data )
{
	m_name = name;
	m_data = data;
}

void MaterialPropertyVector3::Bind( unsigned int programHandle )
{
	GLint idx = glGetUniformLocation( programHandle, m_name.c_str() );
	if( idx  >= 0 )
		glUniform3fv( idx, 1, (GLfloat const *) &m_data );
}

void MaterialPropertyVector3::SetData( void const *data )
{
	m_data = *(Vector3 const *)data;
}

size_t MaterialPropertyVector3::GetByteSize() const
{
	return sizeof( m_data );
}

MaterialPropertyVector4::MaterialPropertyVector4( char const *name, Vector4 const &data )
{
	m_name = name;
	m_data = data;
}

void MaterialPropertyVector4::Bind( unsigned int programHandle )
{
	GLint idx = glGetUniformLocation( programHandle, m_name.c_str() );
	if( idx  >= 0 )
		glUniform4fv( idx, 1, (GLfloat const *) &m_data );
}

void MaterialPropertyVector4::SetData( void const *data )
{
	m_data = *(Vector4 const *)data;
}

size_t MaterialPropertyVector4::GetByteSize() const
{
	return sizeof( m_data );
}

MaterialPropertyRgba::MaterialPropertyRgba( char const *name, Rgba const &data )
{
	m_name = name;
	m_data = data;
}

void MaterialPropertyRgba::Bind( unsigned int programHandle )
{
	Vector4 colorNormalized;
	m_data.GetAsFloats( colorNormalized.x, colorNormalized.y, colorNormalized.z, colorNormalized.w );
	
	GLint idx = glGetUniformLocation( programHandle, m_name.c_str() );
	if( idx  >= 0 )
		glUniform4fv( idx, 1, (GLfloat const *) &colorNormalized );
}

void MaterialPropertyRgba::SetData( void const *data )
{
	m_data = *(Rgba const *)data;
}

size_t MaterialPropertyRgba::GetByteSize() const
{
	return sizeof( m_data );
}
