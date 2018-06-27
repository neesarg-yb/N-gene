#pragma once
#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/DrawCall.hpp"

class Scene;
class Light;
class Camera;
class Texture;
class Renderer;
class Vector3;

class ForwardRenderingPath 
{
public:
	 ForwardRenderingPath( Renderer &activeRenderer );
	~ForwardRenderingPath();

public:
	Renderer	&m_renderer;
	Camera		*m_shadowCamera			= nullptr;
	Texture		*m_shadowColorTarget	= nullptr;
	Texture		*m_shadowDepthTarget	= nullptr;

public:
	void RenderSceneForCamera( Camera &camera, Scene &scene ) const;
	void RenderSceneForShadowMap( Scene &scene ) const;

private:
	void SetMostContributingLights( unsigned int &lightCount, unsigned int (&effectiveLightIndices)[MAX_LIGHTS], Vector3 const &renderablePosition, std::vector< Light* > &lightsInScene ) const;
	void SortDrawCallsForCamera( std::vector< DrawCall > &drawCallsToSort, Camera &camera ) const;
	void SortAlphaDrawCallsForCameraZ( std::vector< DrawCall > &drawCallsToSort, Camera &camera ) const;
	void EnableLightsForDrawCall( DrawCall &dc, std::vector< Light* > &allLights ) const;
};