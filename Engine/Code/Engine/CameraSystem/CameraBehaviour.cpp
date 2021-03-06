#pragma once
#include "CameraBehaviour.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"

CameraBehaviour::CameraBehaviour( std::string const &behaviourName, CameraManager *manager )
	: m_name( behaviourName )
	, m_manager( manager )
{

}

CameraBehaviour::~CameraBehaviour()
{

}

void CameraBehaviour::SetCameraAnchorAndInputSystemTo( Camera *camera, GameObject *anchor, InputSystem *input )
{
	m_camera = camera;
	m_anchor = anchor;
	m_inputSystem = input;
}