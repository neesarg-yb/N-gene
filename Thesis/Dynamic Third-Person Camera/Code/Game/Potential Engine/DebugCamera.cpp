#pragma once
#include "DebugCamera.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Renderer/Renderer.hpp"

DebugCamera::DebugCamera( CameraBehaviour *newBehaviour , bool shadowMapDisabled /*= true*/ )
	: m_behaviour( newBehaviour )
{
	// Setup the Camera
	uint width	= Window::GetInstance()->GetWidth();
	uint height = Window::GetInstance()->GetHeight();

	Texture *colorTarget	= Renderer::GetInstance()->CreateRenderTarget( width, height, TEXTURE_FORMAT_RGBA8 );
	Texture *deapthTarget	= Renderer::GetInstance()->CreateRenderTarget( width, height, TEXTURE_FORMAT_D24S8 );

	SetColorTarget( colorTarget );
	SetDepthStencilTarget( deapthTarget );

	if( m_behaviour != nullptr )
		m_behaviour->SetCameraAnchorAndInputSystemTo( this, nullptr, g_theInput );

	// Setup the overlay camera
	m_uiCamera = new Camera();
	m_uiCamera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_uiCamera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	m_uiCamera->SetProjectionOrtho( 2.f, -1.f, 1.f );			// Make an NDC

	// ShadowMap
	m_shadowMapEnabled = !shadowMapDisabled;
}

DebugCamera::~DebugCamera()
{
	delete m_uiCamera;
	m_uiCamera = nullptr;

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

Texture const* DebugCamera::GetColorTarget() const
{
	return m_outputFramebuffer.m_color_targets[0];
}

void DebugCamera::RenderAsMiniOverlay() const
{
	Renderer* theRenderer = Renderer::GetInstance();

	theRenderer->BindCamera( m_uiCamera );

	// To form an overlay: do not clear screen, make depth of every pixel 1.f, do not write new depth..
	theRenderer->UseShader( nullptr );
	theRenderer->EnableDepth( COMPARE_ALWAYS, false );

	AABB2 rightCornerBounds = m_screenBounds.GetBoundsFromPercentage( Vector2(0.7f, 0.f), Vector2(1.f, 0.3f) );
	AABB2 overlayDrawBounds = rightCornerBounds.GetBoundsFromPercentage( Vector2(0.03f, 0.03f), Vector2(0.97f, 0.97f) );
	Texture const* colorTarget = GetColorTarget();
	theRenderer->DrawAABB( rightCornerBounds, RGBA_WHITE_COLOR );
	theRenderer->DrawTexturedAABB( overlayDrawBounds, *colorTarget, Vector2::ZERO, Vector2::ONE_ONE, RGBA_WHITE_COLOR );
}

void DebugCamera::RenderAsFulscreenOverlay() const
{
	Renderer* theRenderer = Renderer::GetInstance();

	theRenderer->BindCamera( m_uiCamera );

	// To form an overlay: do not clear screen, make depth of every pixel 1.f, do not write new depth..
	theRenderer->UseShader( nullptr );
	theRenderer->EnableDepth( COMPARE_ALWAYS, false );

	AABB2 overlayDrawBounds = m_screenBounds.GetBoundsFromPercentage( Vector2(0.01f, 0.01f), Vector2(0.99f, 0.99f) );
	Texture const* colorTarget = GetColorTarget();
	theRenderer->DrawAABB( m_screenBounds, RGBA_WHITE_COLOR );
	theRenderer->DrawTexturedAABB( overlayDrawBounds, *colorTarget, Vector2::ZERO, Vector2::ONE_ONE, RGBA_WHITE_COLOR );
}
