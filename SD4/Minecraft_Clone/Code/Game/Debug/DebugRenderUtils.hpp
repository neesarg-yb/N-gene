#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Renderer/RenderTypes.hpp"

class Renderer;

class MDebugUtils
{
public:
	static void RenderCubeWireframe		( AABB3 const &worldBounds, Rgba const &color, bool useXRay );
	static void RenderSphereWireframe	( Vector3 const &center, float radius, Rgba const &color, bool useXRay );
	static void RenderVector			( Vector3 const &originPosition, Vector3 const &vector, Rgba const &color, bool useXRay );
};
