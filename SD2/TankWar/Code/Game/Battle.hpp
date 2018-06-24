#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Game/Tank.hpp"
#include "Game/Terrain.hpp"
#include "Game/Bullet.hpp"
#include "Game/Enemy.hpp"

class Material;

typedef std::vector< GameObject* > GameObjectList;

class Battle
{
public:
	 Battle();
	~Battle();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Update( float deltaSeconds );
	void Render() const;

private:
	// Rendering Specific
	static Camera*				s_camera;
	static Scene*				s_battleScene;
	Vector4						m_ambientLight					= Vector4( 1.f, 1.f, 1.f, 0.6f );
	ForwardRenderingPath*		m_renderingPath					= nullptr;
	
	GameObjectList				m_allGameObjects[ NUM_GAME_OBJECT_TYPES ];

	// Battle Specific
	double						m_timeSinceStartOfTheBattle	= 0;
	Terrain*					m_terrain						= nullptr;
	Tank*						m_playerTank					= nullptr;

public:
	// Lights
	static std::vector< Light* > s_lightSources;
	static void	AddNewPointLightToCamareaPosition( Rgba lightColor );

public:
	// Add to Battle
	void	AddNewGameObject( GameObject &newGO );

private:
	double	GetTimeSinceBattleStarted() const;
	void	RotateTheCameraAccordingToPlayerInput	( float deltaSeconds );
	void	ChnageLightAsPerInput					( float deltaSeconds );
	
};