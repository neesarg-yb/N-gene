#pragma once

#include "Tile.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Game/PlayerTank.hpp"
#include "Game/EnemyTank.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/TileDefination.hpp"
#include "Game/EnemyTurret.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Game/Explosion.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include <vector>

typedef std::vector< Entity* > EntityList;

enum EntityType {
	ENTITY_TYPE_ENEMY_TURRET,
	ENTITY_TYPE_ENEMY_TANK,
	ENTITY_TYPE_PLAYER_TANK,
	ENTITY_TYPE_ENEMY_BULLET,
	ENTITY_TYPE_PLAYER_BULLET,
	ENTITY_TYPE_EXPLOSION,
	NUM_ENTITY_TYPES
};

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
	bool debugModeOn = false;
	bool zeroModeEnable = false;
	bool playerIsInvincible = false;

	const std::string pathToTexturePNG = "Data//Images//Terrain_8x8.png";
	Texture* spriteSheetTexture;
	SpriteSheet* theSpriteSheet;
	PlayerTank* theTank;
	EntityList m_entities[ NUM_ENTITY_TYPES ];

	IntVector2 gridSize;
	std::vector< Tile > m_tiles;

	 Map( const IntVector2& gridSize, PlayerTank* cPlayer );
	~Map();

	void BeginFrame();
	void Update( float deltaSeconds );
	void Render();
	void EndFrame();

	void CheckForEntityVsTileCollosion();
	RaycastResult Raycast( const Vector2& startPos, const Vector2& direction, float maxDistance ) const;
	bool HasLineOfSight( const Vector2& startPos, const Vector2& endPos ) const;
	void AddEntityToList( Entity *entity , EntityType type );
	int  GetIndexOfTileAt( int row, int column ) const;
	int  GetIndexOfTileAt( IntVector2 tileCoords ) const;

	void RespawnPlayerTank();

private:

	std::string enemyHitSoundPath = "Data//Audio//EnemyHit.wav";
	SoundID enemyHitSound;
	
	std::string enemyDiedSoundPath = "Data//Audio//EnemyDied.wav";
	SoundID enemyDiedSound;
	
	std::string playerHitSoundPath = "Data//Audio//PlayerHit.wav";
	SoundID playerHitSound;

	std::string playerDiedSoundPath = "Data//Audio//PlayerDied.wav";
	SoundID playerDiedSound;

	Rgba grassColor = Rgba( 000, 255, 000, 255 );
	Rgba stoneColor = Rgba( 128, 128, 128, 255 );

	void PopulateMapWithTiles();
	void DrawTileAt( int row, int column );
	void CorrectEntitysPosition( Entity& thisEntity, Tile& tile );

	void PlaceAllEntitiesRandomly();
	Entity* SpawnNewEntity( EntityType type, Vector2 position, float orientation );				// Factory Function

	void PreventBulletCollisions( float deltaSeconds );
	void CheckPlayerBulletsVsEnemiesCollision();
	void CheckEnemyBulletsVsPlayerTankCollision();
	void CheckPlayerTankVsEnemiesCollision();
	void CheckEnemiesVsEnemiesCollision();
};