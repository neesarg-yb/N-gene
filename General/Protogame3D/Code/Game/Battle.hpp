#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"

class Material;

class Battle
{
public:
	 Battle();
	~Battle();

	void BeginFrame();
	void EndFrame();
	void Update( float deltaSeconds );
	void Render() const;

private:
	double						 m_timeSinceStartOfTheBattle	= 0;
	static Camera*				 s_camera;
	static Scene*				 s_testScene;
	
	ForwardRenderingPath*		 m_renderingPath				= nullptr;
	Renderable*					 m_cube							= nullptr;
	Renderable*					 m_sphere						= nullptr;
	Renderable*					 m_spaceShip					= nullptr;
	Renderable*					 m_spaceShipXRay				= nullptr;
	Renderable*					 m_snowMiku						= nullptr;
	Material*					 m_cubeMaterial					= nullptr;
	Material*					 m_sphereMaterial				= nullptr;
	Material*					 m_spaceShipXRayMaterial		= nullptr;
	Mesh*						 m_testCubeMesh					= nullptr;
	Mesh*						 m_testSphereMesh				= nullptr;
	Mesh*						 m_spaceShipMesh				= nullptr;
	Transform*					 m_spaceShipTransform			= nullptr;
	Transform*					 m_snowMikuTransform			= nullptr;
	Vector4						 m_ambientLight					= Vector4( 1.f, 1.f, 1.f, 0.2f );

public:
	static std::vector< Light* > s_lightSources;
	
	static void	AddNewPointLightToCamareaPosition( Rgba lightColor );

private:
	double	GetTimeSinceBattleStarted() const;
	void	MoveTheCameraAccordingToPlayerInput		( float deltaSeconds );
	void	RotateTheCameraAccordingToPlayerInput	( float deltaSeconds );
	void	ChnageLightAsPerInput					( float deltaSeconds );
	
};