#pragma once
#include <string>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/DebugRenderer/DebugRenderObject.hpp"

class Command;


////////////////////////
// Startup & Shutdown //
////////////////////////
void DebugRendererStartup( Renderer *activeRenderer, Camera *camera3D );	// Uses windowSize to create a camera2D. Where (0,0) is center
void DebugRendererShutdown();


/////////////////////
// Update & Render //
/////////////////////
void DebugRendererUpdate( float deltaSeconds );
void DebugRendererRender();
void ClearAllRenderingObjects( Command& cmd );


//////////////////////
// 2D Render System //
//////////////////////
void DebugRender2DQuad( float lifetime, 
	AABB2 const &bounds,
	Rgba const	&startColor,
	Rgba const	&endColor );

void DebugRender2DLine( float lifetime, 
	Vector2 const	&p0, Rgba const &p0Color,
	Vector2 const	&p1, Rgba const &p1Color,
	Rgba const		&tintStartColor,
	Rgba const		&tintEndColor );

void DebugRender2DText( float lifetime,
	Vector2 const	&position, 
	float const		height, 
	Rgba const		&startColor,	
	Rgba const		&endColor,	
	std::string		asciiText );


//////////////////////
// 3D Render System //
//////////////////////
void DebugRenderPoint( float lifetime, float size,
	Vector3 const	&position, 
	Rgba const		&startColor, 
	Rgba const		&endColor, 
	eDebugRenderMode const mode );

void DebugRenderLineSegment	( float lifetime, 
	Vector3 const	&p0, Rgba const &p0Color, 
	Vector3 const	&p1, Rgba const &p1Color, 
	Rgba const		&startColor, 
	Rgba const		&endColor, 
	eDebugRenderMode const	mode );

void DebugRenderBasis( float lifetime,
	Matrix44 const	&basis, 
	Rgba const		&startColor, 
	Rgba const		&endColor, 
	eDebugRenderMode const mode );		// basis Matrix44's T-Column becomes world position at Basis will be rendered

void DebugRenderSphere( float lifetime, 
	Vector3 const	&pos, 
	float const		radius, 
	Rgba const		&startColor, 
	Rgba const		&endColor, 
	eDebugRenderMode const mode ); 

void DebugRenderWireSphere( float lifetime, 
	Vector3 const	&pos, 
	float const		radius, 
	Rgba const		&startColor, 
	Rgba const		&endColor, 
	eDebugRenderMode const mode );

void DebugRenderWireCube( float lifetime, 
	Vector3 const	&bottomLeftFront,
	Vector3 const	&topRightBack,
	Rgba const		&startColor, 
	Rgba const		&endColor, 
	eDebugRenderMode mode ); 

void DebugRenderQuad( float lifetime, 
	Vector3 const &pos,
	Vector3 const &eulerRotation,
	Vector2 const &xySize,
	Texture *texture,        // default to a white texture if nullptr
	Rgba const &startColor, 
	Rgba const &endColor, 
	eDebugRenderMode mode ); 