#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Renderer/RenderTypes.hpp"
#include "Game/Utility/RaycastResult_MC.hpp"

class Renderer;

class MDebugUtils
{
public:
	static void RenderCubeWireframe		( AABB3 const &worldBounds, Rgba const &color, bool useXRay );
	static void RenderSphereWireframe	( Vector3 const &center, float radius, Rgba const &color, bool useXRay );
	static void RenderLine				( Vector3 const &startPos, Rgba const &startColor, Vector3 const &endPos, Rgba const &endColor, bool useXRay );
	static void RenderBasis				( Vector3 const &position, float length, bool useXRay );
	static void RenderRaycast			( RaycastResult_MC const &raycastResult );
};
