#pragma once
#include "CameraMotionController.hpp"

CameraMotionController::CameraMotionController( char const *name, CameraManager const *manager )
	: m_name( name )
	, m_manager( manager )
{

}

CameraMotionController::~CameraMotionController()
{

}

CameraState CameraMotionController::MoveCamera( CameraState const &currentState, CameraState const &goalState, float deltaSeconds )
{
	UNUSED( currentState );
	UNUSED( deltaSeconds );

	return goalState;
}
