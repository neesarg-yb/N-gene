#pragma once
#include <string>
#include <vector>
#include "Engine/Math/IntVector2.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/MapGenStep.hpp"
#include "Game/Tile.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/Projectile.hpp"
#include "Game/ProjectileDefinition.hpp"
#include "Game/Portal.hpp"

class Camera2D;

const float fSQRT_2_OVER_2 = 1 / sqrtf( 2.f );

const IntVector2 STEP_NORTH (  0 , +1 );
const IntVector2 STEP_SOUTH (  0 , -1 );
const IntVector2 STEP_EAST  ( +1 ,  0 );
const IntVector2 STEP_WEST  ( -1 ,  0 );
const IntVector2 STEP_NE	( +1 , +1 );
const IntVector2 STEP_NW	( +1 , -1 );
const IntVector2 STEP_SE	( -1 , +1 );
const IntVector2 STEP_SW	( -1 , -1 );

const Vector2	 DIRECTION_NORTH (  0.f , +1.f );
const Vector2	 DIRECTION_SOUTH (  0.f , -1.f );
const Vector2	 DIRECTION_EAST  ( +1.f ,  0.f );
const Vector2	 DIRECTION_WEST  ( -1.f ,  0.f );
const Vector2	 DIRECTION_NE    ( +fSQRT_2_OVER_2 ,  +fSQRT_2_OVER_2 );
const Vector2	 DIRECTION_NW    ( +fSQRT_2_OVER_2 ,  -fSQRT_2_OVER_2 );
const Vector2	 DIRECTION_SE    ( -fSQRT_2_OVER_2 ,  +fSQRT_2_OVER_2 );
const Vector2	 DIRECTION_SW    ( -fSQRT_2_OVER_2 ,  -fSQRT_2_OVER_2 );

typedef std::vector< Entity* > EntityList;

const float RAYCAST_STEPS_PER_TILE = 10.f;

struct RaycastResult {
	RaycastResult( bool didImpact, const Vector2& impactPosition, const IntVector2& impactTileCoords, float fractionTravelled, const Vector2& impactNormal );

	bool		m_didImpact;
	Vector2		m_impactPosition;
	IntVector2	m_impactTileCoords;
	float		m_atFractionTravelled;
	Vector2		m_impactNormal;
};

class Map
{
public:
	 Map( std::string mapName, std::string mapDefinitionName, Player& thePlayer );
	~Map();

	std::string			m_name = "No name set";
	IntVector2			m_dimension = IntVector2( 0, 0 );
	MapDefinition*		m_definition = nullptr;

	std::vector<Tile>	m_tiles;
	Player&				m_currentPlayer;
	EntityList			m_entities[ NUM_ENTITY_TYPES ];
	EntityList			m_allEntities;

	void BeginFrame();
	void Update( float deltaSeconds );
	void Render();
	void EndFrame();

	void				RenderTiles();
	std::vector< int >	GetIndexeOfTilesVisableInCamera( Camera2D& activeCamera );
	void				DrawTileAtCoordinate( int x, int y );							// Note: It doesn't support dual texture Tiles
	int					GetIndexOfTileAt( IntVector2 tileCoords ) const;
	void				RenderAllPortals();
	void				SortAndRenderAllEntities();										// Note: It doesn't render Portals

	Entity* CreateNewEntity( EntityType type, std::string definitionName, Vector2& position, float orientationDegrees );
	void	AddEntityToList( Entity *entity , EntityType type );
	void	CreateActorsOnWalkableTile	( std::string definition, int numActors );
	void	CreateActorOnTileType		( std::string definition, std::string tileType, int numActors );
	void	MoveActorToAWalkableTile	( Actor* actor = nullptr );						// As default it will move currentPlayer to walkableTile

	void CreatePortal( std::string portalType, std::string onTileType, std::string  destinationMap, std::string  reciprocalPortalType );
	std::vector< Portal* > GetAllPortals();
	
	void CheckForTileVsEntityCollosion();												// It doesn't include Projectile
	void UpdateProjectileWithCollision( float deltaSeconds );							// Uses Predictive Physics
	void CorrectEntitysPosition( Entity& thisEntity, Tile& tile );
	void CheckForProjectileVsActorCollision();
	void CheckForPortalVsActorCollision();
	void CheckForActorVsActorCollision();
	
	RaycastResult Raycast( const Vector2& startPos, const Vector2& direction, float maxDistance ) const;
	bool HasLineOfSight( const Vector2& startPos, const Vector2& endPos ) const;

private:
	void DeleteEntitiesReadyToDie();													// Note: It doesn't delete the Player

	EntityType GetEntityTypeFromDefinitionName( std::string definitionName );
};