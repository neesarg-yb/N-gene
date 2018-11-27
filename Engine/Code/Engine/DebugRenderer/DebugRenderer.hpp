#pragma once
#include <string>
#include <functional>
#include "Engine/Core/RaycastResult.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/DebugRenderer/DebugRenderObject.hpp"

class Camera;
class Command;

typedef std::function< float(float x) > xyCurve_cb;


////////////////////////
// Startup & Shutdown //
////////////////////////
void DebugRendererStartup( Renderer *activeRenderer );	// Uses windowSize to create a camera2D. Where (0,0) is center
void DebugRendererShutdown();

/////////////////////
// Update & Render //
/////////////////////

//
// Execution order should be:
//
// .----------------------------.    then     .--------------------------.
// |  DebugRenderBeginFrame();  |  ========>  | Add DebugRenderObject(s) |
// *----------------------------*             *--------------------------*
//                                                        |               
//                                                        | then          
//                                                        V               
//  otherwise drawing a debug object         .----------------------------.
//  just for one frame wont work!            |  DebugRenderLateRender();  |
//  (by setting lifetime = 0.f)              *----------------------------*
//                                                                        
void DebugRendererBeginFrame( Clock const *clock );		// Deletes the overdue Render Objects
void DebugRendererLateRender( Camera *camera3D );
void ClearAllRenderingObjects( Command& cmd );


//////////////////////
// 2D Render System //
//////////////////////
void DebugRender2DRound( float lifetime,
	Vector2 const	&center,
	float const		 radius,
	Rgba const		&startColor,
	Rgba const		&endColor );

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

FloatRange DebugRenderXYCurve( float lifetime,
	AABB2 const		&drawBounds,
	xyCurve_cb		curveCB,
	FloatRange		xRange,
	float			step,
	Rgba const		&curveColor,
	Rgba const		&backgroundColor,
	Rgba const		&gridlineColor );		// Returns FloatRange of Y-Axis values


//////////////////////
// 3D Render System //
//////////////////////
void DebugRenderPoint( float lifetime, float size,
	Vector3 const	&position, 
	Rgba const		&startColor, 
	Rgba const		&endColor, 
	eDebugRenderMode const mode );

void DebugRenderLineSegment( float lifetime, 
	Vector3 const	&p0, Rgba const &p0Color, 
	Vector3 const	&p1, Rgba const &p1Color, 
	Rgba const		&startColor, 
	Rgba const		&endColor, 
	eDebugRenderMode const	mode );

void DebugRenderVector( float lifetime,
	Vector3 const	&origin,
	Vector3 const	&vector,
	Rgba const		&color,
	Rgba const		&startColor,
	Rgba const		&endColor,
	eDebugRenderMode const mode );

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

void DebugRenderTag( float lifetime,
	float	const height,
	Vector3 const &startPos,
	Vector3 const &upDirection,
	Vector3 const &rightDirection,
	Rgba	const &startColor,
	Rgba	const &endColor,
	std::string	asciiText );

void DebugRenderRaycast( float lifetime,
	Vector3			const &startPosition,
	RaycastResult	const &raycastResult,
	float			const impactPointSize,
	Rgba			const &colorOnImpact,
	Rgba			const &colorOnNoImpact,
	Rgba			const &impactPositionColor,
	Rgba			const &impactNormalColor,
	Rgba			const &startColor,
	Rgba			const &endColor,
	eDebugRenderMode mode );

void DebugRenderCamera( float lifetime,
	Camera			const &camera,
	float			const cameraBodySize,
	Rgba			const &frustumColor,
	Rgba			const &startColor,
	Rgba			const &endColor,
	eDebugRenderMode mode );
