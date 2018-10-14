#pragma once
#include "CameraManager.hpp"

CameraManager::CameraManager( Camera &camera, InputSystem &inputSystem )
	: m_camera( camera )
	, m_cameraRadius( 2.f * camera.GetCameraNear() )
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
	m_lastSuggestedState = m_aciveBehaviour->Update( deltaSeconds );

	// Constrains, if enabled
	if( m_constrainsEnabled == true )
	{
		// From lower priority to higher
		CameraConstrainPriorityQueue copiedConstrainQueue( m_activeConstrains );

		// Apply all
		while ( copiedConstrainQueue.size() > 0 )
		{
			CameraConstrain *constrain = copiedConstrainQueue.top();
			copiedConstrainQueue.pop();

			constrain->Execute( m_lastSuggestedState );
		}
	}

	// This might change if in transition
	CameraState updatedCameraState = m_lastSuggestedState;

	// Camera Behavior Transition
	if( m_behaviourTransitionTimeRemaining > 0.f )
	{
		float t = ( m_behaviourTransitionSeconds - m_behaviourTransitionTimeRemaining ) / m_behaviourTransitionSeconds;
		t = ClampFloat01(t);

		// Interpolate slowly to the suggested position, over time
		updatedCameraState = CameraState::Interpolate( m_stateOnTransitionBegin, m_lastSuggestedState, t );
		m_behaviourTransitionTimeRemaining -= deltaSeconds;
	}

	// Sets properties of the camera
	m_camera.SetFOVForPerspective( updatedCameraState.m_fov );
	m_camera.SetCameraPositionTo ( updatedCameraState.m_position );
	m_camera.SetCameraQuaternionRotationTo( updatedCameraState.m_orientation );
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

void CameraManager::SetRaycastCallback( raycast_std_func raycastFunction )
{
	m_raycastCB = raycastFunction;
}

void CameraManager::SetSphereCollisionCallback( sphere_collision_func collisionFunction )
{
	m_collisionCB = collisionFunction;
}

float CameraManager::GetCameraRadius() const
{
	return m_cameraRadius;
}

CameraContext CameraManager::GetCameraContext() const
{
	return CameraContext( m_anchor->m_transform.GetWorldPosition(), m_raycastCB, m_collisionCB );
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
		m_stateOnTransitionBegin			= m_lastSuggestedState;
		m_behaviourTransitionTimeRemaining	= m_behaviourTransitionSeconds;
	}
	
	// Sets the new camera behavior to active
	int idx = GetCameraBehaviourIndex( behaviourName );
	m_aciveBehaviour = m_cameraBehaviours[ idx ];

	// Update Active Constrains
	Tags const &constrainsToActivate = m_aciveBehaviour->m_constrains;
	ResetActivateConstrainsFromTags( constrainsToActivate );
}

/*
	// To cache elements of the priority queue
	CameraConstrainPriorityQueue cachedQueue;
	
	// Delete if this constrain already exists
	while ( m_registeredConstrains.size() > 0 )
	{
		CameraConstrain *thisConstrain( m_registeredConstrains.top() );
		m_registeredConstrains.pop();
		
		if( thisConstrain->m_name == newConstrain->m_name )
		{
			delete thisConstrain;
			thisConstrain = nullptr;
		}
		else
			cachedQueue.push( thisConstrain );
	}
	
	// Add new constrain
	cachedQueue.push( newConstrain );
	
	// Swap the updated contents of cached queue to original one
	std::swap( cachedQueue, m_registeredConstrains );
*/

void CameraManager::RegisterConstrain( CameraConstrain* newConstrain )
{
	// Delete if we have a registered constrain of the same name..
	for( uint i = 0; i < m_registeredConstrains.size(); i++ )
	{
		if( m_registeredConstrains[i]->m_name == newConstrain->m_name )
		{
			delete m_registeredConstrains[i];
			m_registeredConstrains[i] = nullptr;

			m_registeredConstrains.erase( m_registeredConstrains.begin() + i );
			break;
		}
	}

	// Add new constrain
	m_registeredConstrains.push_back( newConstrain );
}

void CameraManager::DeregisterConstrain( char const *name )
{
	// Delete if we have a registered constrain of the same name..
	for( uint i = 0; i < m_registeredConstrains.size(); i++ )
	{
		if( m_registeredConstrains[i]->m_name == name )
		{
			delete m_registeredConstrains[i];
			m_registeredConstrains[i] = nullptr;

			m_registeredConstrains.erase( m_registeredConstrains.begin() + i );
			break;
		}
	}
}

void CameraManager::EnableConstrains( bool enable /*= true */ )
{
	m_constrainsEnabled = enable;
}

void CameraManager::ResetActivateConstrainsFromTags( Tags const &constrainsToActivate )
{
	// Reset the priority queue
	m_activeConstrains = CameraConstrainPriorityQueue();

	// Add constrains which needs to be active
	Strings allConstrains;
	constrainsToActivate.GetTags( allConstrains );
	for each (std::string constrainName in allConstrains)
	{
		int idx = GetCameraConstrainIndex( constrainName );
		if( idx < 0 )
			continue;

		m_activeConstrains.push( m_registeredConstrains[idx] );
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

int CameraManager::GetCameraConstrainIndex( std::string const &constrainName )
{
	int idx = -1;

	for( int constrainIdx = 0; constrainIdx < m_registeredConstrains.size(); constrainIdx++ )
	{
		if( m_registeredConstrains[constrainIdx]->m_name == constrainName )
		{
			idx = constrainIdx;
			break;
		}
	}

	return idx;
}

