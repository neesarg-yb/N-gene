#pragma once
#include "CameraManager.hpp"

CameraManager::CameraManager( Camera &camera, InputSystem &inputSystem )
	: m_camera( camera )
	, m_inputSystem( inputSystem )
{

}

CameraManager::~CameraManager()
{

}

void CameraManager::Update( float deltaSeconds )
{
	// Get suggested target point from active Camera Behavior
	CameraTargetPoint suggestedTargetPoint = m_aciveBehaviour->Update( deltaSeconds );

	// Snap to suggested target point, for now
	TODO( "Set Camera FOV!" );
	m_camera.SetCameraPositionTo( suggestedTargetPoint.m_position );
	m_camera.SetCameraQuaternionRotationTo( suggestedTargetPoint.m_orientation );
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

void CameraManager::AddCameraBehaviour( CameraBehaviour *newCameraBehaviour )
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
		m_cameraBehaviours.push_back( newCameraBehaviour );
}

void CameraManager::RemoveCameraBehaviour( std::string const &behaviourName )
{
	int idx = GetCameraBehaviourIndex( behaviourName );
	
	// No behavior found
	if( idx < 0 )
		return;

	// Fast remove
	std::swap( m_cameraBehaviours.back(), m_cameraBehaviours[idx] );
	delete m_cameraBehaviours.back();
	m_cameraBehaviours.back() = nullptr;
	m_cameraBehaviours.pop_back();
}

void CameraManager::RemoveCameraBehaviour( CameraBehaviour *cameraBehaviourToRemove )
{
	RemoveCameraBehaviour( cameraBehaviourToRemove->m_name );
}

void CameraManager::SetActiveCameraBehaviourTo( std::string const &behaviourName )
{
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

