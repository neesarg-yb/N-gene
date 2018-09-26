#pragma once
#include "CameraManager.hpp"

CameraManager::CameraManager( Camera &camera, InputSystem &inputSystem )
	: m_camera( camera )
	, m_inputSystem( inputSystem )
{

}

CameraManager::~CameraManager()
{
	// Delete all the camera behaviors
	while( m_cameraBehaviours.size() > 0 )
	{
		// Fast delete
		std::swap( m_cameraBehaviours.front(), m_cameraBehaviours.back() );
		delete m_cameraBehaviours.back();
		m_cameraBehaviours.back() = nullptr;

		m_cameraBehaviours.pop_back();
	}
}

void CameraManager::Update( float deltaSeconds )
{
	// Normal Update
	if( m_behaviourTransitionTimeRemaining <= 0.f )
	{
		// Get suggested target point from active Camera Behavior
		m_lastSuggestedPoint = m_aciveBehaviour->Update( deltaSeconds );

		// Snap to suggested target point, for now
		m_camera.SetFOVForPerspective( m_lastSuggestedPoint.m_fov );
		m_camera.SetCameraPositionTo ( m_lastSuggestedPoint.m_position );
		m_camera.SetCameraQuaternionRotationTo( m_lastSuggestedPoint.m_orientation );
	}
	else // Interpolate to target point
	{
		float t = ( m_behaviourTransitionSeconds - m_behaviourTransitionTimeRemaining ) / m_behaviourTransitionSeconds;
		t = ClampFloat01(t);

		m_behaviourTransitionTimeRemaining -= deltaSeconds;
		m_lastSuggestedPoint				= m_aciveBehaviour->Update( deltaSeconds );

		CameraTargetPoint interTargetPoint = CameraTargetPoint::Interpolate( m_targetPointOnTransitionBegin, m_lastSuggestedPoint, t );

		m_camera.SetFOVForPerspective( interTargetPoint.m_fov );
		m_camera.SetCameraPositionTo ( interTargetPoint.m_position );
		m_camera.SetCameraQuaternionRotationTo( interTargetPoint.m_orientation );
	}
}

void CameraManager::PreUpdate()
{
	m_aciveBehaviour->PreUpdate();
}

void CameraManager::PostUpdate()
{
	m_aciveBehaviour->PostUpdate();
}

void CameraManager::SetAnchor( GameObject *anchor )
{
	m_anchor = anchor;
}

int CameraManager::AddNewCameraBehaviour( CameraBehaviour *newCameraBehaviour )
{
	int idx = GetCameraBehaviourIndex( newCameraBehaviour );
	if( idx >= 0 )
	{
		// If behavior of same name exists, replace it
		delete m_cameraBehaviours[ idx ];
		m_cameraBehaviours[ idx ] = nullptr;
		m_cameraBehaviours[ idx ] = newCameraBehaviour;
	}
	else // If not, add a new behavior
	{
		m_cameraBehaviours.push_back( newCameraBehaviour );
		idx = (int)m_cameraBehaviours.size() - 1;
	}

	m_cameraBehaviours[idx]->SetCameraAnchorAndInputSystemTo( &m_camera, m_anchor, &m_inputSystem );
	return idx;
}

void CameraManager::DeleteCameraBehaviour( std::string const &behaviourName )
{
	int idx = GetCameraBehaviourIndex( behaviourName );
	
	// No behavior found
	if( idx < 0 )
		return;

	// Remove Camera & Anchor References
	m_cameraBehaviours[idx]->SetCameraAnchorAndInputSystemTo( nullptr, nullptr, nullptr );

	// Fast Delete
	std::swap( m_cameraBehaviours.back(), m_cameraBehaviours[idx] );
	delete m_cameraBehaviours.back();
	m_cameraBehaviours.back() = nullptr;
	m_cameraBehaviours.pop_back();
}

void CameraManager::DeleteCameraBehaviour( CameraBehaviour *cameraBehaviourToDelete )
{
	DeleteCameraBehaviour( cameraBehaviourToDelete->m_name );
}

void CameraManager::SetActiveCameraBehaviourTo( std::string const &behaviourName )
{
	int idx = GetCameraBehaviourIndex( behaviourName );
	m_aciveBehaviour = m_cameraBehaviours[ idx ];

	TODO( "Fix it!" );
	if( m_cameraBehaviours.size() > 1U )
	{
		m_targetPointOnTransitionBegin		= m_lastSuggestedPoint;
		m_behaviourTransitionTimeRemaining	= m_behaviourTransitionSeconds;
	}
}

int CameraManager::GetCameraBehaviourIndex( CameraBehaviour *cb )
{
	std::string cbName = cb->m_name;
	return GetCameraBehaviourIndex( cbName );
}

int CameraManager::GetCameraBehaviourIndex( std::string const &behaviourName )
{
	int idx = -1;

	for( int behaviourIdx = 0; behaviourIdx < m_cameraBehaviours.size(); behaviourIdx++ )
	{
		if( m_cameraBehaviours[behaviourIdx]->m_name == behaviourName )
		{
			idx = behaviourIdx;
			break;
		}
	}

	return idx;
}

