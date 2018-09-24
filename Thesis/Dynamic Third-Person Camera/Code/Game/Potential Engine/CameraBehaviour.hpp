#pragma once
#include <string>
#include "Game/Potential Engine/CameraTargetPoint.hpp"

class CameraBehaviour
{
public:
	 CameraBehaviour( std::string const &behaviourName );
	~CameraBehaviour();

public:
	std::string m_name = "NAME NOT ASSIGNED!";

public:
	void PreUpdate();
	void PostUpdate();
	CameraTargetPoint Update( float deltaSeconds );
};