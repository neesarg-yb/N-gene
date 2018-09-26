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
	m_lastSuggestedPoint = m_aciveBehaviour->Update( deltaSeconds );

	// This might change if in transition
	CameraTargetPoint updatedTargetPoint = m_lastSuggestedPoint;

	// Camera Behavior Transition
	if( m_behaviourTransitionTimeRemaining > 0.f )
	{
		float t = ( m_behaviourTransitionSeconds - m_behaviourTransitionTimeRemaining ) / m_behaviourTransitionSeconds;
		t = ClampFloat01(t);

		// Interpolate slowly to the suggested position, over time
		updatedTargetPoint = CameraTargetPoint::Interpolate( m_targetPointOnTransitionBegin, m_lastSuggestedPoint, t );
		m_behaviourTransitionTimeRemaining -= deltaSeconds;
	}

	// Sets properties of the camera
	m_camera.SetFOVForPerspective( updatedTargetPoint.m_fov );
	m_camera.SetCameraPositionTo ( updatedTargetPoint.m_position );
	m_camera.SetCameraQuaternionRotationTo( updatedTargetPoint.m_orientation );
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
	// If there is a camera behavior already active
	if( m_aciveBehaviour != nullptr )
	{
		// Start interpolation towards new behavior
		m_targetPointOnTransitionBegin		= m_lastSuggestedPoint;
		m_behaviourTransitionTimeRemaining	= m_behaviourTransitionSeconds;
	}
	
	// Sets the new camera behavior to active
	int idx = GetCameraBehaviourIndex( behaviourName );
	m_aciveBehaviour = m_cameraBehaviours[ idx ];
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

