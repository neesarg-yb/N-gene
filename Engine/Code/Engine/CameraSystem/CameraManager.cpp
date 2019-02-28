#pragma once
#include "CameraManager.hpp"
#include "Engine/Profiler/Profiler.hpp"

#define DEFAULT_MOTION_CONTROLLER_NAME "default"

CameraManager::CameraManager( Camera &camera, InputSystem &inputSystem, float cameraRadius )
	: m_camera( camera )
	, m_cameraRadius( cameraRadius )
	, m_inputSystem( inputSystem )
	, m_previousCameraStates( CAMERASTATE_HISTORY_LENGTH )
	, m_defaultMotionController( DEFAULT_MOTION_CONTROLLER_NAME, this )
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

	// Erase the map
	m_motionControllers.clear();
}

void CameraManager::Update( float deltaSeconds )
{
	PROFILE_SCOPE_FUNCTION();

	m_lastSuggestedState = m_aciveBehaviour->Update( deltaSeconds, m_currentCameraState );

	// Constraints, if enabled
	if( m_constraintsEnabled == true )
	{
		// From lower priority to higher
		CameraConstraintPriorityQueue copiedConstraintQueue( m_activeConstraints );

		// Apply all
		while ( copiedConstraintQueue.size() > 0 )
		{
			CameraConstraint *constraint = copiedConstraintQueue.top();
			copiedConstraintQueue.pop();

			constraint->Execute( m_lastSuggestedState );
		}
	}

	// This might change if in transition
	CameraState constrainedCameraState = m_lastSuggestedState;

	// Camera Motion Controller
	if( m_behaviourTransitionTimeRemaining <= 0.f )
	{
		// If not in transition of changing Camera Behaviour..
		constrainedCameraState = GetActiveMotionController()->MoveCamera( m_currentCameraState, constrainedCameraState, deltaSeconds );
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
		constrainedCameraState = m_defaultMotionController.MoveCamera( m_currentCameraState, constrainedCameraState, deltaSeconds );
		SetCurrentCameraStateTo( constrainedCameraState );
	}

	// Add the final camera state to history
	m_previousCameraStates.AddNewEntry( constrainedCameraState );
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
	CameraState lastFramesCameraState = CameraState();		// In case this is the first frame

	// If not, get most recent entry from the history
	if( m_previousCameraStates.IsNotEmpty() )
		lastFramesCameraState = m_previousCameraStates.GetRecentEntry( 0 );

	return CameraContext( m_anchor, m_raycastCB, m_cameraRadius, m_collisionCB, lastFramesCameraState );
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
	// If setting the behavior for the first time
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
		m_previousCameraStates.AddNewEntry( suggestedState );
	}

	// Update Active Constraints
	Tags const &constraintsToActivate = m_aciveBehaviour->m_constraints;
	ResetActivateConstraintsFromTags( constraintsToActivate );
}

std::string CameraManager::GetActiveCameraBehaviorName() const
{
	return m_aciveBehaviour->m_name;
}

void CameraManager::RegisterConstraint( CameraConstraint* newConstraint )
{
	// Delete if we have a registered constraint of the same name..
	for( uint i = 0; i < m_registeredConstraints.size(); i++ )
	{
		if( m_registeredConstraints[i]->m_name == newConstraint->m_name )
		{
			delete m_registeredConstraints[i];
			m_registeredConstraints[i] = nullptr;

			m_registeredConstraints.erase( m_registeredConstraints.begin() + i );
			break;
		}
	}

	// Add new constraint
	m_registeredConstraints.push_back( newConstraint );
}

void CameraManager::DeregisterConstraint( char const *name )
{
	// Delete if we have a registered constraint of the same name..
	for( uint i = 0; i < m_registeredConstraints.size(); i++ )
	{
		if( m_registeredConstraints[i]->m_name == name )
		{
			delete m_registeredConstraints[i];
			m_registeredConstraints[i] = nullptr;

			m_registeredConstraints.erase( m_registeredConstraints.begin() + i );
			break;
		}
	}
}

void CameraManager::EnableConstraints( bool enable /*= true */ )
{
	m_constraintsEnabled = enable;
}

void CameraManager::RegisterMotionController( CameraMotionController *motionController )
{
	std::string controllerName = motionController->m_name;
	GUARANTEE_RECOVERABLE( controllerName != DEFAULT_MOTION_CONTROLLER_NAME, "CameraManager: Trying to REPLACE the default motion controller! Be careful." );

	m_motionControllers[ controllerName ] = motionController;
}

void CameraManager::DeregisterMotionController( char const *name )
{
	CameraMotionControllerMap::iterator it = m_motionControllers.find( name );
	if( it != m_motionControllers.end() )
		m_motionControllers.erase( it );
}

void CameraManager::SetAverageCountForInputReferenceMatrixCalculation( int avgCount )
{
	m_averageWithNumPreviousCameraStates = (uint)avgCount;
}

CameraState CameraManager::GetCameraStateForInputReference() const
{
	return m_previousCameraStates.GetProgressiveAverageOfRecentEntries( m_averageWithNumPreviousCameraStates );
}

Matrix44 CameraManager::GetCameraMatrixForInputReference() const
{
	CameraState	const averagedState	= GetCameraStateForInputReference();
	Vector3		const worldPosition	= averagedState.m_position;
	Quaternion	const rotation		= averagedState.m_orientation;

	Matrix44 cameraMatrix = rotation.GetAsMatrix44();
	cameraMatrix.SetTColumn( worldPosition );

	return cameraMatrix;
}

CameraMotionController* CameraManager::GetActiveMotionController()
{
	CameraMotionControllerMap::iterator mcIt = m_motionControllers.find( m_aciveBehaviour->m_motionControllerName );

	if( mcIt == m_motionControllers.end() )
		return &m_defaultMotionController;			// If not found, return the default one
	else
		return mcIt->second;						// Else, return the found one
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

void CameraManager::ResetActivateConstraintsFromTags( Tags const &constraintsToActivate )
{
	// Reset the priority queue
	m_activeConstraints = CameraConstraintPriorityQueue();

	// Add constraints which needs to be active
	Strings allConstraints;
	constraintsToActivate.GetTags( allConstraints );
	for each (std::string constraintName in allConstraints)
	{
		int idx = GetCameraConstraintIndex( constraintName );
		if( idx < 0 )
			continue;

		m_activeConstraints.push( m_registeredConstraints[idx] );
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

int CameraManager::GetCameraConstraintIndex( std::string const &constraintName )
{
	int idx = -1;

	for( int constraintIdx = 0; constraintIdx < m_registeredConstraints.size(); constraintIdx++ )
	{
		if( m_registeredConstraints[constraintIdx]->m_name == constraintName )
		{
			idx = constraintIdx;
			break;
		}
	}

	return idx;
}

