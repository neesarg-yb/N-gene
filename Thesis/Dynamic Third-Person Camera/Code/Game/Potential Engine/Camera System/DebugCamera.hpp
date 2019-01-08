#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Game/Potential Engine/Camera System/CameraBehaviour.hpp"

class DebugCamera : public Camera
{
public:
	 DebugCamera( CameraBehaviour *newBehaviour, bool shadowMapDisabled = true );
	~DebugCamera();

private:
	CameraBehaviour *m_behaviour			= nullptr;

	// UI Overlay
	Camera			*m_uiCamera				= nullptr;
	Vector2 const	 m_screenBottomLeft		= Vector2( -g_aspectRatio, -1.f );
	Vector2 const	 m_screenTopRight		= Vector2(  g_aspectRatio,  1.f );
	AABB2	const	 m_screenBounds			= AABB2  ( m_screenBottomLeft, m_screenTopRight );

public:
	void			Update();
	Texture const*	GetColorTarget() const;

public:
	void		RenderAsMiniOverlay() const;
	void		RenderAsFulscreenOverlay() const;
};
