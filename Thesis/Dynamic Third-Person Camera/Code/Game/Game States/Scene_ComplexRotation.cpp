#pragma once
#include "Scene_ComplexRotation.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/theGame.hpp"

Scene_ComplexRotation::Scene_ComplexRotation( Clock *parentClock )
	: GameState( "COMPLEX ROTATION", parentClock )
{

}

Scene_ComplexRotation::~Scene_ComplexRotation()
{

}

void Scene_ComplexRotation::JustFinishedTransition()
{

}

void Scene_ComplexRotation::BeginFrame()
{
	DebugRendererBeginFrame( m_clock );
}

void Scene_ComplexRotation::EndFrame()
{

}

void Scene_ComplexRotation::Update()
{
	float const deltaSeconds = (float)m_clock->GetFrameDeltaSeconds();


	// Change Target according to input
	if( g_theInput->WasKeyJustPressed( VK_Codes::ENTER ) )
		m_chasingTheTarget = true;

	float rotationDir = 0.f;	// +1 for CW, -1 for CCW
	if( g_theInput->IsKeyPressed( VK_Codes::UP ) )
		rotationDir -= 1.f;
	if( g_theInput->IsKeyPressed( VK_Codes::DOWN ) )
		rotationDir += 1.f;

	Complex targetRotator = Complex( rotationDir * m_rotationSpeed * 1.7f * deltaSeconds );
	m_targetRotation *= targetRotator;


	// Rotate the current complex number
	if( m_chasingTheTarget )
	{
		float maxRotationThisFrame = m_rotationSpeed * deltaSeconds;
		m_currentRotation.TurnToward( m_targetRotation, maxRotationThisFrame );

		if( m_currentRotation.GetRotation() == m_targetRotation.GetRotation() )
			m_chasingTheTarget = false;
	}
	

	// ESC - Go back to Menu
	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		g_theGame->StartTransitionToState( "LEVEL SELECT" );
}

void Scene_ComplexRotation::Render( Camera *gameCamera ) const
{
	g_theRenderer->BindCamera( gameCamera );
	g_theRenderer->ClearColor( RGBA_BLACK_COLOR );
	g_theRenderer->ClearDepth( 1.f );

	float	xyAxisLength		= 400.f;
	float	vectorLength		= 100.f;
	Vector2 currRotationVector	= Vector2( m_currentRotation.r, m_currentRotation.i );
	Vector2 targRotationVector	= Vector2( m_targetRotation.r, m_targetRotation.i );

	// XY Axis
	DebugRender2DLine( 0.f, Vector2( -xyAxisLength, 0.f), RGBA_GRAY_COLOR, Vector2( +xyAxisLength, 0.f), RGBA_GRAY_COLOR, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR );
	DebugRender2DLine( 0.f, Vector2( 0.f, -xyAxisLength), RGBA_GRAY_COLOR, Vector2( 0.f, +xyAxisLength), RGBA_GRAY_COLOR, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR );

	// Rotation Vector
	DebugRender2DLine( 0.f, Vector2::ZERO, RGBA_KHAKI_COLOR, targRotationVector * vectorLength, RGBA_KHAKI_COLOR, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR );
	DebugRender2DLine( 0.f, Vector2::ZERO, RGBA_WHITE_COLOR, currRotationVector * vectorLength, RGBA_RED_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR );

	// Current Rotation Text
	Vector2 currRotTextLocation	= Vector2( 20.f, -(vectorLength + 20.f) );
	std::string curRotText = Stringf( "%.2f", m_currentRotation.GetRotation() );
	DebugRender2DText( 0.f, currRotTextLocation, 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, Stringf( "Current Rotation: %7s", curRotText.c_str() ) );

	// Target Rotation Text
	Vector2 targetRotTextLocation = Vector2( currRotTextLocation.x, currRotTextLocation.y - 20.f );
	std::string targetRotText = Stringf( "%.2f", m_targetRotation.GetRotation() );
	DebugRender2DText( 0.f, targetRotTextLocation, 15.f, RGBA_KHAKI_COLOR, RGBA_KHAKI_COLOR, Stringf( "Target Rotation : %7s", targetRotText.c_str() ) );

	// Instruction on how to use
	DebugRender2DText( 0.f, Vector2(-750.f, 450.f), 15.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, Stringf( "[UP] [DOWN] : Move the target rotation" ) );
	DebugRender2DText( 0.f, Vector2(-750.f, 430.f), 15.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, Stringf( "[ENTER]     : Rotate main hand towards the target" ) );

	DebugRendererLateRender( gameCamera );
}

