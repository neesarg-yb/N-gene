#pragma once
#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/DrawCall.hpp"

class Scene;
class Light;
class Camera;
class Renderer;
class Vector3;

class ForwardRenderingPath 
{
public:
	 ForwardRenderingPath( Renderer &activeRenderer );
	~ForwardRenderingPath();

public:
	Renderer &m_renderer;

public:
	void RenderSceneForCamera( Camera &camera, Scene &scene ) const;

private:
	void SetMostContributingLights( unsigned int &lightCount, unsigned int (&effectiveLightIndices)[MAX_LIGHTS], Vector3 const &renderablePosition, std::vector< Light* > &lightsInScene ) const;
	void SortDrawCallsForCamera( std::vector< DrawCall > &drawCallsToSort, Camera &camera ) const;
	void SortAlphaDrawCallsForCameraZ( std::vector< DrawCall > &drawCallsToSort, Camera &camera ) const;
	void EnableLightsForDrawCall( DrawCall &dc, std::vector< Light* > &allLights ) const;
};