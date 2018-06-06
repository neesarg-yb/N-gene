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

Vector3 Transform::GetWorldPosition() const
{
	// If no parent
	if( m_parent == nullptr )
		return m_position;

	// Get position relative to parent
	Matrix44 worldTransformMat	= GetWorldTransformMatrix();
	Vector3	 worldPosition		= worldTransformMat.GetTColumn();

	return worldPosition;
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

void Transform::SetParentAs( Transform const *parent )
{
	m_parent = parent;
}

void Transform::AddChild( Transform *child )
{
	m_children.push_back( child );
}

void Transform::RemoveChild( Transform *childToRemove )
{
	for( uint childIdx = 0; childIdx < m_children.size(); childIdx++ )
	{
		if( m_children[childIdx] == childToRemove )
			m_children.erase( m_children.begin() + childIdx );
	}
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

Matrix44 Transform::GetParentTransform() const
{
	if( m_parent == nullptr )
		return Matrix44();

	return m_parent->GetTransformMatrix();
}

Matrix44 Transform::GetWorldTransformMatrix() const
{
	if( m_parent == nullptr )
		return GetTransformMatrix();
	else
	{
		Matrix44 transformMatrix = m_parent->GetWorldTransformMatrix();
		transformMatrix.Append( GetTransformMatrix() );
		return transformMatrix;
	}
}
