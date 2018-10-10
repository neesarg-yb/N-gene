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
	m_lastSuggestedState = m_aciveBehaviour->Update( deltaSeconds );

	// Constrains, if enabled
	if( m_constrainsEnabled == true )
	{
		// Apply all
		for each (CameraConstrain* constrain in m_activeConstrains)
			constrain->Execute( m_lastSuggestedState );
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

CameraContext CameraManager::GetCameraContext() const
{
	return CameraContext( m_anchor->m_transform.GetWorldPosition(), m_raycastCB );
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

void CameraManager::RegisterConstrain( char const *name, CameraConstrain *newConstrain )
{
	CameraConstrainMap::iterator it = m_registeredConstrains.find( name );
	if( it != m_registeredConstrains.end() )
	{
		// If this constrain already exists, erase the it
		delete it->second;
		it->second = nullptr;

		m_registeredConstrains.erase( it );
	}

	// Add new constrain
	m_registeredConstrains[ name ] = newConstrain;
}

void CameraManager::DeregisterConstrain( char const *name )
{
	CameraConstrainMap::iterator it = m_registeredConstrains.find( name );

	// Not found? => return
	if( it == m_registeredConstrains.end() )
		return;
	else
	{
		// Found, delete it!
		delete it->second;
		it->second = nullptr;

		m_registeredConstrains.erase( it );
	}
}

void CameraManager::EnableConstrains( bool enable /*= true */ )
{
	m_constrainsEnabled = enable;
}

void CameraManager::ResetActivateConstrainsFromTags( Tags const &constrainsToActivate )
{
	// Remove all the active constrains
	m_activeConstrains.clear();

	// Add constrains which needs to be active
	Strings allConstrains;
	constrainsToActivate.GetTags( allConstrains );
	for each (std::string constrainName in allConstrains)
	{
		CameraConstrainMap::iterator it = m_registeredConstrains.find( constrainName );
		if( it != m_registeredConstrains.end() )
			m_activeConstrains.push_back( it->second );
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

