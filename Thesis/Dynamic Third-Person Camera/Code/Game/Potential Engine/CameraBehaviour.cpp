#pragma once
#include "CameraBehaviour.hpp"

CameraBehaviour::CameraBehaviour( std::string const &behaviourName )
	: m_name( behaviourName )
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