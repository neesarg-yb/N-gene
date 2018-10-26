#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Game/Potential Engine/CameraBehaviour.hpp"

class DebugCamera : public Camera
{
public:
	 DebugCamera( CameraBehaviour *newBehaviour );
	~DebugCamera();

private:
	CameraBehaviour *m_behaviour = nullptr;

public:
	void		Update();
	Texture*	GetColorTarget();
};
