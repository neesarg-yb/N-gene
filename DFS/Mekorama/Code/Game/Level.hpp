#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Renderer/PickBuffer.hpp"
#include "Game/GameObject.hpp"
#include "Game/World/Tower.hpp"
#include "Game/World/Block.hpp"
#include "Game/World/Robot.hpp"
#include "Game/World/LevelDefinition.hpp"

typedef std::vector< GameObject* > GameObjectList;

struct BlockDragData
{
	Pipe*		anchorPipe		= nullptr;
	Vector2		startMousePos	= Vector2::ZERO;
	Vector3		startBlockPos	= Vector3::ZERO;

	BlockDragData() { }
	BlockDragData( Pipe *isOnPipe, Vector2 &mousePos, Vector3 &blockPos )
	{
		anchorPipe		= isOnPipe;
		startMousePos	= mousePos;
		startBlockPos	= blockPos;
	}
};

class Material;

class Level
{
public:
	 Level( std::string definitionName, Robot &playerRobot );
	~Level();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Update( float deltaSeconds );
	void Render() const;

public:
	LevelDefinition const		&m_definition;

private:
	// Rendering Specific
	OrbitCamera*				m_camera					= nullptr;
	Scene*						m_levelScene				= nullptr;
	Vector4						m_ambientLight				= Vector4( 1.f, 1.f, 1.f, 0.7f );
	ForwardRenderingPath*		m_renderingPath				= nullptr;
	Camera*						m_uiCamera					= nullptr;
	AABB2 const					m_uiDrawBounds				= AABB2( -g_aspectRatio, -1.f, g_aspectRatio, 1.f );
	Rgba const					m_uiBackgroundColor			= Rgba( 0, 0, 0, 200 );
	bool						m_puzzleSolved				= false;

public:
	// Lights
	std::vector< Light* >		m_lightSources;

public:
	// Tower Specific
	GameObjectList				m_allGameObjects;
	Robot&						m_playerRobot;
	Tower*						m_tower						= nullptr;

private:
	// Local
	double						m_timeSinceStartOfTheBattle	= 0;

	// PickBuffer
	PickBuffer					m_pickBuffer;

	// Gameplay Specific
	Block*						m_targetBlock				= nullptr;
	Block*						m_dragBlock					= nullptr;
	BlockDragData				m_dragDataAtStart;

private:
	double	GetTimeSinceBattleStarted() const;
	void	RotateTheCameraAccordingToPlayerInput	( float deltaSeconds );
	void	ChnageLightAsPerInput					( float deltaSeconds );

	// Gameplay Specific
	void	ChangeTargetBlockOnMouseClick();
	Block*	GetBlockFromMousePosition();
	float	GetDragDistanceOnPipe( Block &dragableBlock, Vector2 const &mousePos, BlockDragData const &startDragData );
	
	IntVector3	GetFinishPositionInTower() const;
	void		ShowPuzzleSolved() const;
};