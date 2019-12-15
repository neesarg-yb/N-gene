#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/CameraSystem/CameraBehaviour.hpp"

class DebugCamera : public Camera
{
public:
	 DebugCamera( CameraBehaviour *newBehaviour, InputSystem *inputSystem, bool shadowMapDisabled = true );
	~DebugCamera();

private:
	CameraBehaviour *m_behaviour			= nullptr;
	InputSystem		*m_inputSystem			= nullptr;

	// UI Overlay
	Camera			*m_uiCamera				= nullptr;
	Vector2 const	 m_screenBottomLeft;
	Vector2 const	 m_screenTopRight;
	AABB2	const	 m_screenBounds			= AABB2  ( m_screenBottomLeft, m_screenTopRight );

public:
	void			Update();
	Texture const*	GetColorTarget() const;

public:
	void		RenderAsMiniOverlay() const;
	void		RenderAsFulscreenOverlay() const;
};
