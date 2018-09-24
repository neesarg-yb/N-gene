#pragma once
#include "CameraBehaviour.hpp"

CameraBehaviour::CameraBehaviour( std::string const &behaviourName )
	: m_name( behaviourName )
{

}

CameraBehaviour::~CameraBehaviour()
{

}

void CameraBehaviour::PreUpdate()
{

}

void CameraBehaviour::PostUpdate()
{

}

CameraTargetPoint CameraBehaviour::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	return CameraTargetPoint();
}

