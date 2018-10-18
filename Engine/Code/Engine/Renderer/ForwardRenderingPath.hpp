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
class Sampler;
class Shader;

class ForwardRenderingPath 
{
public:
	 ForwardRenderingPath( Renderer &activeRenderer );
	~ForwardRenderingPath();

public:
	Renderer	&m_renderer;
	Camera		*m_shadowCamera			= nullptr;
	Shader		*m_shadowShader			= nullptr;
	Sampler		*m_shadowSampler		= nullptr;
	Texture		*m_shadowColorTarget	= nullptr;
	Texture		*m_shadowDepthTarget	= nullptr;

public:
	void RenderSceneForCamera( Camera &camera, Scene &scene ) const;
	void RenderSceneForShadowMap( Scene &scene, Vector3 const &sceneCameraPosition ) const;

private:
	void SetMostContributingLights( unsigned int &lightCount, unsigned int (&effectiveLightIndices)[MAX_LIGHTS], Vector3 const &renderablePosition, std::vector< Light* > &lightsInScene ) const;
	void SortDrawCallsForCamera( std::vector< DrawCall > &drawCallsToSort, Camera &camera ) const;
	void SortAlphaDrawCallsForCameraZ( std::vector< DrawCall > &drawCallsToSort, Camera &camera ) const;
	void EnableLightsForDrawCall( DrawCall &dc, std::vector< Light* > &allLights ) const;
};