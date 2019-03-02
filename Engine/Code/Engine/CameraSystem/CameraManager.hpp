#pragma once
#include <queue>
#include <vector>
#include "Engine/Core/Tags.hpp"
#include "Engine/Core/RaycastResult.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/CameraSystem/CameraState.hpp"
#include "Engine/CameraSystem/CameraContext.hpp"
#include "Engine/CameraSystem/CameraBehaviour.hpp"
#include "Engine/CameraSystem/CameraConstraint.hpp"
#include "Engine/CameraSystem/CameraStateHistory.hpp"
#include "Engine/CameraSystem/CameraMotionController.hpp"

struct CustomCameraConstarintCompare
{
	// Such that top element is the lowest priority, and bottom is the highest priority
	bool operator() ( CameraConstraint const *lhs, CameraConstraint const *rhs ) const
	{
		return (*lhs > *rhs);
	}
};

typedef std::vector< CameraBehaviour* >																	CameraBehaviourList;
typedef std::vector< CameraConstraint* >																CameraConstraintList;
typedef std::priority_queue< CameraConstraint*, CameraConstraintList, CustomCameraConstarintCompare >	CameraConstraintPriorityQueue;		// Top: lowest priority & Bottom: highest priority
typedef std::map< std::string, CameraMotionController* >												CameraMotionControllerMap;

constexpr int CAMERASTATE_HISTORY_LENGTH = 60;

class CameraManager
{
public:
	 CameraManager( Camera &camera, InputSystem &inputSystem, float cameraRadius );
	~CameraManager();

private:
	// Current Camera State
	CameraState				 m_currentCameraState;

	// Camera Context
	InputSystem				&m_inputSystem;				// Current input system
	Camera					&m_camera;					// Possessed Camera
	float					 m_cameraRadius = 1.f;		// It should be based on Camera's near-plane
	GameObject				*m_anchor		= nullptr;	// Anchor GameObject
	raycast_std_func		 m_raycastCB	= nullptr;	// Raycast function, provided from Game Side
	sphere_collision_func	 m_collisionCB	= nullptr;	// Sphere Collision function, provided from Game Side

	// Camera Behaviours
	CameraBehaviourList		 m_cameraBehaviours;		// All the behaviors that can be run on Camera
	std::string				 m_cameraBehaviorToActivate;
	CameraBehaviour			*m_activeBehaviour = nullptr;
	CameraState				 m_lastSuggestedState;		// It is set before applying any constraints or motion controller changes
	CameraStateHistoy		 m_previousCameraStates;	// It is set at the end of Update(), after all the contraint and motion controller changes
	CameraState				 m_stateOnTransitionBegin;

	// Camera's State for game-side input's reference
	uint					 m_averageWithNumPreviousCameraStates = 1;
	
	float 					 m_behaviourTransitionSeconds		= 1.f;
	float					 m_behaviourTransitionTimeRemaining = 0.f;

	// Camera Constraints
	bool							m_constraintsEnabled = true;
	CameraConstraintList			m_registeredConstraints;	// Stored without any sorting by priority
	CameraConstraintPriorityQueue	m_activeConstraints;		// Lowest priority at top, highest at bottom

	// All Registered Motion Controllers
	CameraMotionController			m_defaultMotionController;
	CameraMotionControllerMap		m_motionControllers;

public:
	void Update( float deltaSeconds );
	void PreUpdate();
	void PostUpdate();

public:
	CameraState		GetCurrentCameraState() const;

	// Context
	void			SetAnchor( GameObject *anchor );
	void			SetRaycastCallback( raycast_std_func raycastFunction );
	void			SetSphereCollisionCallback( sphere_collision_func collisionFunction );
	CameraContext	GetCameraContext() const;
	
	// Behaviours
	int				AddNewCameraBehaviour( CameraBehaviour *newCameraBehaviour );						// Now I'll manage this behavior, including deletion
	void			DeleteCameraBehaviour( CameraBehaviour *cameraBehaviourToDelete );					// Deletes it
	void			DeleteCameraBehaviour( std::string const &behaviourName );							// Deletes it
	void			ChangeCameraBehaviourTo( std::string const &behaviourName, float transitionTime );	// If another behavior already active, it changes the new behavior gets activated next frame..
	std::string		GetActiveCameraBehaviorName() const;

	// Constraints
	void			RegisterConstraint( CameraConstraint* newConstraint );
	void			DeregisterConstraint( char const *name );
	void			EnableConstraints( bool enable = true );

	// Motion Controller
	void			RegisterMotionController( CameraMotionController *motionController );		// Sets m_activeMotionController
	void			DeregisterMotionController( char const *name );

	// Reference for game-side player input
	void			SetAverageCountForInputReferenceMatrixCalculation( int avgCount );
	CameraState		GetCameraStateForInputReference() const;
	Matrix44		GetCameraMatrixForInputReference() const;

private:
	CameraMotionController* GetActiveMotionController();

	void SetCurrentCameraStateTo( CameraState newState );
	void SetActiveCameraBehaviourTo( std::string const &behaviourName );
	void ResetActivateConstraintsFromTags( Tags const &constraintsToActivate );

private:
	int GetCameraBehaviourIndex ( CameraBehaviour *cb );				// Returns -1 if couldn't find it in the list
	int GetCameraBehaviourIndex ( std::string const &behaviourName );	// Returns -1 if couldn't find it in the list
	int GetCameraConstraintIndex( std::string const &constraintName );	// Returns -1 if couldn't find it in the list
};
