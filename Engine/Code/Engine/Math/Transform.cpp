#pragma once
#include "Transform.hpp"

Transform::Transform( Vector3 const &position, Vector3 const &rotation, Vector3 const &scale )
	: m_position( position )
	, m_rotation( rotation )
	, m_scale( scale )
	, m_isDirty( true )
{
	RecalculateTheMatrix();
}

Vector3 Transform::GetPosition() const
{
	return m_position;
}

Vector3 Transform::GetRotation() const
{
	return m_rotation;
}

Vector3 Transform::GetScale() const
{
	return m_scale;
}

Matrix44 Transform::GetTransformMatrix() const
{
	if( m_isDirty != true )
		return m_transformMatrix;
	else
	{
		RecalculateTheMatrix();
		return m_transformMatrix;
	}
}

void Transform::SetPosition( Vector3 const &position )
{
	m_position	= position;
	m_isDirty	= true;
}

void Transform::SetRotation( Vector3 const &rotation )
{
	m_rotation	= rotation;
	m_isDirty	= true;
}

void Transform::SetScale( Vector3 const &scale )
{
	m_scale		= scale;
	m_isDirty	= true;
}

void Transform::RecalculateTheMatrix() const
{
	// Set current Transform Matrix to Identity
	m_transformMatrix.SetIdentity();
	m_transformMatrix.Translate3D( m_position );		// Append the Translate Matrix
	m_transformMatrix.RotateDegrees3D( m_rotation );	// Append the Rotation	Matrix
	m_transformMatrix.Scale3D( m_scale );				// Append the Scale		Matrix

	m_isDirty = false;
}