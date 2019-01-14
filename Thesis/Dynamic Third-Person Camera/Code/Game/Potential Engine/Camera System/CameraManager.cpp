#pragma once
#include "CameraManager.hpp"
#include "Engine/Profiler/Profiler.hpp"

CameraManager::CameraManager( Camera &camera, InputSystem &inputSystem, float cameraRadius )
	: m_camera( camera )
	, m_cameraRadius( cameraRadius )
	, m_inputSystem( inputSystem )
{
	m_defaultMotionController = new CameraMotionController( "default", this );
}

CameraManager::~CameraManager()
{
	delete m_defaultMotionController;
	m_defaultMotionController = nullptr;

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
	PROFILE_SCOPE_FUNCTION();

	m_lastSuggestedState = m_aciveBehaviour->Update( deltaSeconds, m_currentCameraState );

	// Constrains, if enabled
	if( m_constrainsEnabled == true )
	{
		// From lower priority to higher
		CameraConstrainPriorityQueue copiedConstrainQueue( m_activeConstrains );

		// Apply all
		while ( copiedConstrainQueue.size() > 0 )
		{
			CameraConstraint *constrain = copiedConstrainQueue.top();
			copiedConstrainQueue.pop();

			constrain->Execute( m_lastSuggestedState );
		}
	}

	// This might change if in transition
	CameraState constrainedCameraState = m_lastSuggestedState;

	// Camera Motion Controller
	if( m_behaviourTransitionTimeRemaining <= 0.f )
	{
		// If not in transition of changing Camera Behaviour..
		constrainedCameraState = GetMotionController()->MoveCamera( m_currentCameraState, constrainedCameraState, deltaSeconds );
		SetCurrentCameraStateTo( constrainedCameraState );
	}
	else
	{
		// Camera Behavior Transition is ongoing..
		float t = ( m_behaviourTransitionSeconds - m_behaviourTransitionTimeRemaining ) / m_behaviourTransitionSeconds;
		t = ClampFloat01(t);

		// Interpolate slowly to the suggested position, over time
		constrainedCameraState = CameraState::Interpolate( m_stateOnTransitionBegin, m_lastSuggestedState, t );
		m_behaviourTransitionTimeRemaining -= deltaSeconds;

		// Update the current state
		constrainedCameraState = m_defaultMotionController->MoveCamera( m_currentCameraState, constrainedCameraState, deltaSeconds );
		SetCurrentCameraStateTo( constrainedCameraState );
	}

	m_lastFinalCameraState = constrainedCameraState;
}

void CameraManager::PreUpdate()
{
	m_aciveBehaviour->PreUpdate();
}

void CameraManager::PostUpdate()
{
	m_aciveBehaviour->PostUpdate();
}

CameraState CameraManager::GetCurrentCameraState() const
{
	return m_currentCameraState;
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

CameraContext CameraManager::GetCameraContext() const
{
	return CameraContext( m_anchor, m_raycastCB, m_cameraRadius, m_collisionCB, m_lastFinalCameraState );
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
	// If setting the behaviour for the first time
	bool immediatlySetCurrentState = false;

	// If there is a camera behavior already active
	if( m_aciveBehaviour != nullptr )
	{
		// Start interpolation towards new behavior
		m_stateOnTransitionBegin			= m_lastSuggestedState;
		m_behaviourTransitionTimeRemaining	= m_behaviourTransitionSeconds;
	}
	else
	{
		immediatlySetCurrentState = true;
	}
	
	// Sets the new camera behavior to active
	int idx = GetCameraBehaviourIndex( behaviourName );
	m_aciveBehaviour = m_cameraBehaviours[ idx ];

	// Change Camera Position to wherever that behavior wants it to
	if( immediatlySetCurrentState )
	{
		CameraState suggestedState = m_aciveBehaviour->Update( 0.f, m_currentCameraState );
		
		SetCurrentCameraStateTo( suggestedState );
		m_lastFinalCameraState = suggestedState;
	}

	// Update Active Constrains
	Tags const &constrainsToActivate = m_aciveBehaviour->m_constrains;
	ResetActivateConstrainsFromTags( constrainsToActivate );
}

void CameraManager::SetActiveMotionControllerTo( CameraMotionController *motionController )
{
	// Set it
	m_activeMotionController = motionController;

	// For safety, change its manager to this!
	m_activeMotionController->m_manager = this;
}

void CameraManager::RegisterConstrain( CameraConstraint* newConstrain )
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

CameraMotionController* CameraManager::GetMotionController()
{
	return (m_activeMotionController == nullptr) ? m_defaultMotionController : m_activeMotionController;
}

void CameraManager::SetCurrentCameraStateTo( CameraState newState )
{
	// Sets properties of the camera
	m_camera.SetFOVForPerspective( newState.m_fov );
	m_camera.SetCameraPositionTo ( newState.m_position );
	m_camera.SetCameraQuaternionRotationTo( newState.m_orientation );

	// Store off the current state
	m_currentCameraState = newState;
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

