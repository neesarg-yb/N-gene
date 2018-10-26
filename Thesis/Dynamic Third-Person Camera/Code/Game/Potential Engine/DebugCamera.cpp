#pragma once
#include "DebugCamera.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Renderer/Renderer.hpp"

DebugCamera::DebugCamera( CameraBehaviour *newBehaviour )
	: m_behaviour( newBehaviour )
{
	uint width	= Window::GetInstance()->GetWidth();
	uint height = Window::GetInstance()->GetHeight();

	Texture *colorTarget	= Renderer::GetInstance()->CreateRenderTarget( width, height, TEXTURE_FORMAT_RGBA8 );
	Texture *deapthTarget	= Renderer::GetInstance()->CreateRenderTarget( width, height, TEXTURE_FORMAT_D24S8 );

	SetColorTarget( colorTarget );
	SetDepthStencilTarget( deapthTarget );

	if( m_behaviour != nullptr )
		m_behaviour->SetCameraAnchorAndInputSystemTo( this, nullptr, g_theInput );
}

DebugCamera::~DebugCamera()
{
	if( m_behaviour != nullptr )
	{
		delete m_behaviour;
		m_behaviour = nullptr;
	}
}

void DebugCamera::Update()
{
	if( m_behaviour == nullptr )
		return;

	float deltaSeconds = (float) GetMasterClock()->GetFrameDeltaSeconds();

	// Get new Camera State, suggested by the Camera Behaviour
	CameraState	cameraState  = CameraState( Vector3::ZERO, m_cameraTransform.GetPosition(), m_cameraTransform.GetRotation(), GetFOV() );
	cameraState = m_behaviour->Update( deltaSeconds, cameraState );
	cameraState.m_position  += cameraState.m_velocity * deltaSeconds;

	// Set the Camera State
	SetFOVForPerspective			( cameraState.m_fov );
	SetCameraPositionTo				( cameraState.m_position );
	SetCameraQuaternionRotationTo	( cameraState.m_orientation );
}

Texture* DebugCamera::GetColorTarget()
{
	return m_outputFramebuffer.m_color_targets[0];
}
