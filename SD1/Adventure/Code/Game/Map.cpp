#pragma once
#include "Map.hpp"
#include "Game/theGame.hpp"
#include "Game/Camera2D.hpp"
#include <algorithm>

RaycastResult::RaycastResult( bool didImpact, const Vector2& impactPosition, const IntVector2& impactTileCoords, float fractionTravelled, const Vector2& impactNormal ) {
	m_didImpact				= didImpact;
	m_impactPosition		= impactPosition;
	m_impactTileCoords		= impactTileCoords;
	m_atFractionTravelled	= fractionTravelled;
	m_impactNormal			= impactNormal;
}

Map::Map( std::string mapName, std::string mapDefinitionName, Player& thePlayer )
	: m_name( mapName )
	, m_currentPlayer( thePlayer )
{
	// Get MapDefinition from name..
	std::map< std::string, MapDefinition* >::iterator it = MapDefinition::s_definitions.find( mapDefinitionName );
	GUARANTEE_OR_DIE( it != MapDefinition::s_definitions.end() , "MapDefination " + mapDefinitionName + " not found..!" );
	
	// Set m_definition..
	MapDefinition* mapDefinition = it->second;
	m_definition = mapDefinition;

	// Set Map's width & height
	m_dimension = IntVector2( mapDefinition->m_width, mapDefinition->m_height );

	// Setup each tile to defaultTile & give its coordinates
	for( int rowNum = 0; rowNum < m_dimension.y; rowNum++ )
	{
		for( int columnNum = 0; columnNum < m_dimension.x; columnNum++ )
		{
			IntVector2 currTileCoords = IntVector2( columnNum, rowNum );
			Tile newTile = Tile( currTileCoords, m_definition->m_defaultTile );
			m_tiles.push_back( newTile );
		}
	}

	// Call Run() on each MapGenStep of the mapDefinition
	for( int steps = 0; steps < (int) m_definition->m_mapGenSteps.size(); steps++ )
	{
		MapGenStep* thisStep = m_definition->m_mapGenSteps[ steps ];
		thisStep->Run( *this );
	}

	// Add Player to EntityList
	AddEntityToList( &m_currentPlayer, ENTITY_TYPE_PLAYER );
	// MoveActorToAWalkableTile( &m_currentPlayer );
	m_currentPlayer.m_position = Vector2( 5.f, 5.f );

}

Map::~Map()
{

}

void Map::BeginFrame()
{

}

void Map::Update( float deltaSeconds )
{
	for( int entityType = 0; entityType < NUM_ENTITY_TYPES; entityType++ )
	{
		// Do not update projectiles, they work on Predictive Physics
		if( entityType != ENTITY_TYPE_PROJECTILE )
		{
			// Update all other entities
			for( Entity* currentEntity : m_entities[ entityType ] )
			{
				currentEntity->Update(deltaSeconds);
			}
		}
	}

	UpdateProjectileWithCollision( deltaSeconds );			// Updates all Projectiles

	CheckForTileVsEntityCollosion();
	CheckForActorVsActorCollision();
	CheckForProjectileVsActorCollision();
}

void Map::Render()
{
	// Draw all tiles
	RenderTiles();

	// Draw all Portals
	RenderAllPortals();

	// Render all other Entities
	SortAndRenderAllEntities();
}

void Map::EndFrame()
{
	DeleteEntitiesReadyToDie();


	// TODO: Check for Victory Conditions
	std::string winOnDeathOf		= g_theGame->m_currentAdventure->m_definition->m_winOnDeathOf;
	std::string winOnCollectionOf	= g_theGame->m_currentAdventure->m_definition->m_winOnCollectionOf;

	if( winOnDeathOf != "" )
	{
		// Check if that Actor is dead or not..
	}

	if( winOnCollectionOf != "" )
	{
		// Check if that Item is collected or not..
	}


	CheckForPortalVsActorCollision();
}

void Map::RenderTiles()
{
	// Store all vertexes in this vector
	std::vector< Vertex_3DPCU > verts;

	// For all visible Tiles
	std::vector< int > visibleTileIndexes = GetIndexeOfTilesVisableInCamera( *g_theGame->m_theCamera );

	// Push all visible Tile's PCU in verts..
	for( int index : visibleTileIndexes )
	{
		Tile thisTile		= m_tiles[ index ];
		AABB2 bounds		= thisTile.GetWorldBounds();
		AABB2 texUVBounds	= thisTile.m_tileDef->m_baseTileUVTextBounds;
		Rgba  baseTint		= thisTile.m_tileDef->m_baseTileSpriteTint;

		Vertex_3DPCU newVert = Vertex_3DPCU( Vector2( bounds.mins.x, bounds.maxs.y ).GetAsVector3(), baseTint, Vector2( texUVBounds.mins.x, texUVBounds.maxs.y ) );			// Upper-left
								verts.push_back( newVert );
					 newVert = Vertex_3DPCU( Vector2( bounds.maxs.x, bounds.maxs.y ).GetAsVector3(), baseTint, Vector2( texUVBounds.maxs.x, texUVBounds.maxs.y ) );			// Upper-right
								verts.push_back( newVert );
					 newVert = Vertex_3DPCU( Vector2( bounds.maxs.x, bounds.mins.y ).GetAsVector3(), baseTint, Vector2( texUVBounds.maxs.x, texUVBounds.mins.y ) );			// Bottom-right
								verts.push_back( newVert );
					 newVert = Vertex_3DPCU( Vector2( bounds.mins.x, bounds.mins.y ).GetAsVector3(), baseTint, Vector2( texUVBounds.mins.x, texUVBounds.mins.y ) );			// Bottom-left
								verts.push_back( newVert );
		
		if( thisTile.m_tileDef->m_overlayEnabled == true )
		{
			AABB2 overlayTexUVBounds	= thisTile.m_tileDef->m_overlayTileUVTextBounds;
			Rgba  overlayTint			= thisTile.m_tileDef->m_overlayTileSpriteTint;

			// Pushback overlay tile, as well
					newVert = Vertex_3DPCU( Vector2( bounds.mins.x, bounds.maxs.y ).GetAsVector3(), overlayTint, Vector2( overlayTexUVBounds.mins.x, overlayTexUVBounds.maxs.y ) );			// Upper-left
							verts.push_back( newVert );
					newVert = Vertex_3DPCU( Vector2( bounds.maxs.x, bounds.maxs.y ).GetAsVector3(), overlayTint, Vector2( overlayTexUVBounds.maxs.x, overlayTexUVBounds.maxs.y ) );			// Upper-right
							verts.push_back( newVert );
					newVert = Vertex_3DPCU( Vector2( bounds.maxs.x, bounds.mins.y ).GetAsVector3(), overlayTint, Vector2( overlayTexUVBounds.maxs.x, overlayTexUVBounds.mins.y ) );			// Bottom-right
							verts.push_back( newVert );
					newVert = Vertex_3DPCU( Vector2( bounds.mins.x, bounds.mins.y ).GetAsVector3(), overlayTint, Vector2( overlayTexUVBounds.mins.x, overlayTexUVBounds.mins.y ) );			// Bottom-left
							verts.push_back( newVert );
		}
	}

	// Draw all the verts in single render call!
	g_theRenderer->DrawTexturedAABBArray( verts.data(), verts.size(), g_tileSpriteSheet->m_spriteSheetTexture );

	// Draw heatMap values
	for( int index : visibleTileIndexes)
	{
		AABB2 drawBounds	= m_tiles[index].GetWorldBounds();
		float heatValue		= m_currentPlayer.m_currentHeatMap->GetHeat( m_tiles[index].m_tileCoords );

		g_theRenderer->DrawTextInBox2D( std::to_string( (int)heatValue ), Vector2(1.f, 0.f), drawBounds, 0.1f, RGBA_RED_COLOR, g_theGame->newBMF, TEXT_DRAW_SHRINK_TO_FIT );
	}
}

std::vector< int > Map::GetIndexeOfTilesVisableInCamera( Camera2D& activeCamera )
{
	std::vector< int > indexesToReturn;

	// Get Camera bounds
	AABB2		cameraBounds		= activeCamera.GetCameraBounds();
	IntVector2	bottomLeftCoord		= cameraBounds.mins;
	IntVector2	topRightCoord		= cameraBounds.maxs;

	// Loop through all tiles that are within bounds..
	// Note: It will try to take one more tile on every edge, which is not in camera view..
	for( int y = bottomLeftCoord.y - 1; y <= topRightCoord.y; y++ )
	{
		for( int x = bottomLeftCoord.x - 1; x <= topRightCoord.x; x++ )
		{
			// Add it to indexesToReturn
			int index = GetIndexFromColumnRowNumberForMatrixOfWidth( x, y, m_dimension.x );
			if( index >= 0 && index < m_dimension.x * m_dimension.y )		// If invalid index is caught, don't push it
				indexesToReturn.push_back( index );
		}
	}

	return indexesToReturn;
}

void Map::DrawTileAtCoordinate( int x, int y )
{
	int index = GetIndexFromColumnRowNumberForMatrixOfWidth( x, y, m_dimension.x );

	Tile thisTile		= m_tiles[ index ];

	AABB2 boundsOfTile	= thisTile.GetWorldBounds();
	AABB2 texUVBounds	= thisTile.m_tileDef->m_baseTileUVTextBounds;
	Rgba  tileTint		= thisTile.m_tileDef->m_baseTileSpriteTint;

	g_theRenderer->DrawTexturedAABB(boundsOfTile, g_tileSpriteSheet->m_spriteSheetTexture, texUVBounds.mins, texUVBounds.maxs, tileTint );
}

Entity* Map::CreateNewEntity( EntityType type, std::string definitionName, Vector2& position, float orientationDegrees )
{
	Entity *toReturn = nullptr;

	switch (type)
	{
	case ENTITY_TYPE_NP_ACTOR:
		toReturn = new Actor( position, orientationDegrees, definitionName );
		static_cast<Actor*>(toReturn)->m_wanderAndShootPlayer = true;			// Note: Assuming that all non-player actors do this behavior
		break;
	case ENTITY_TYPE_PLAYER:
		toReturn = new Player( position, orientationDegrees, definitionName );
		break;
	default:
		GUARANTEE_OR_DIE( false, std::string("Factory Function: No appropriate EntityType found to create..!") );
		break;
	}

	return toReturn;
}

void Map::AddEntityToList( Entity *entity , EntityType type ) {
	bool entityAlreadyExist = false;
	// If you find same entity in List
	for( const Entity* entityInList : m_entities[ type ] ) {
		// don't add it
		if( entityInList == entity ) {
			entityAlreadyExist = true;
			return;
		}
	}

	// If not found
	if( entityAlreadyExist == false ) {
		// add it
		m_entities[ type ].push_back(entity);
		m_allEntities.push_back(entity);
	}
}

EntityType Map::GetEntityTypeFromDefinitionName( std::string definitionName )
{
	// TODO: Remove this thing, it is contradicting the concept of creating new Type from XML without touching the code!

	EntityType type = NUM_ENTITY_TYPES;
	if( definitionName == "Goblin" ||
		definitionName == "Clown"  ||
		definitionName == "Friend"	)
	{
		type = ENTITY_TYPE_NP_ACTOR;
	}
	else if ( definitionName == "Player" )
	{
		type = ENTITY_TYPE_PLAYER;
	}
	else if ( definitionName == "Fire")
	{
		type = ENTITY_TYPE_PROJECTILE;
	}
	else if ( definitionName == "TeleportVortex" )
	{
		type = ENTITY_TYPE_PORTAL;
	}

	return type;
}

void Map::CreateActorsOnWalkableTile( std::string definition, int numActors )
{
	EntityType type = GetEntityTypeFromDefinitionName( definition );

	// Get vector of index of all tiles which are walkable
	std::vector< int > availableTilesIndex;
	for( int i = 0; i < (m_dimension.x * m_dimension.y) - 1.f; i++ )
	{
		if( m_tiles[i].m_tileDef->m_allowsWalking == true && m_tiles[i].m_tileExtraInfo->m_tags.HasTags("entitySpawned") == false  )
		{
			availableTilesIndex.push_back( i );
		}
	}

	if( availableTilesIndex.size() > 0 )
	{
		// For loop through numActors
		for( int g = 0; g < numActors; g++ )
		{
			Entity* actorToSpawn = CreateNewEntity( type, definition, Vector2::ZERO, 0.f );

			// Choose a random index
			int randIndex = GetRandomIntInRange( 0, availableTilesIndex.size() - 1 );
			// Spawn an Actor
			Vector2 tilePosition = Vector2( (float)m_tiles[ availableTilesIndex[randIndex] ].m_tileCoords.x, (float)m_tiles[ availableTilesIndex[randIndex] ].m_tileCoords.y );
			Vector2 spawnPosition = tilePosition + Vector2( 0.5f, 0.5f );
			actorToSpawn->m_position = spawnPosition;
			AddEntityToList( actorToSpawn, type );

			// Add Tag to tile that "actorSpawned"
			m_tiles[ availableTilesIndex[randIndex] ].m_tileExtraInfo->m_tags.SetOrRemoveTags("entitySpawned");

			// Erase the used randIndex from availableTilesIndex
			availableTilesIndex.erase( availableTilesIndex.begin() + randIndex );
		}
	}
}

void Map::CreateActorOnTileType( std::string definition, std::string tileType, int numActors )
{
	EntityType type = GetEntityTypeFromDefinitionName( definition );

	// Get vector of index of all tiles which are walkable
	std::vector< int > availableTilesIndex;
	for( int i = 0; i < (m_dimension.x * m_dimension.y) - 1.f; i++ )
	{
		if( m_tiles[i].m_tileDef->m_tileName == tileType && m_tiles[i].m_tileExtraInfo->m_tags.HasTags("entitySpawned") == false )
		{
			availableTilesIndex.push_back( i );
		}
	}

	if( availableTilesIndex.size() > 0 )
	{
		// For loop through numActors
		for( int g = 0; g < numActors; g++ )
		{
			Entity* actorToSpawn = CreateNewEntity( type, definition, Vector2::ZERO, 0.f );

			// Choose a random index
			int randIndex = GetRandomIntInRange( 0, availableTilesIndex.size() - 1 );
			// Spawn an Actor
			Vector2 tilePosition = Vector2( (float)m_tiles[ availableTilesIndex[randIndex] ].m_tileCoords.x, (float)m_tiles[ availableTilesIndex[randIndex] ].m_tileCoords.y );
			Vector2 spawnPosition = tilePosition + Vector2( 0.5f, 0.5f );
			actorToSpawn->m_position = spawnPosition;
			AddEntityToList( actorToSpawn, type );

			// Add Tag to tile that "actorSpawned"
			m_tiles[ availableTilesIndex[randIndex] ].m_tileExtraInfo->m_tags.SetOrRemoveTags("entitySpawned");

			// Erase the used randIndex from availableTilesIndex
			availableTilesIndex.erase( availableTilesIndex.begin() + randIndex );
		}
	}
}

void Map::MoveActorToAWalkableTile( Actor* actor /*= nullptr*/ )
{
	// Get vector of index of all tiles which are walkable
	std::vector< int > availableTilesIndex;
	for( int i = 0; i < (m_dimension.x * m_dimension.y) - 1.f; i++ )
	{
		if( m_tiles[i].m_tileDef->m_allowsWalking == true )
		{
			availableTilesIndex.push_back( i );
		}
	}

	// Choose a random index
	int randIndex = GetRandomIntInRange( 0, availableTilesIndex.size() - 1 );
	// Move an Actor
	Vector2 tilePosition = Vector2( (float)m_tiles[ availableTilesIndex[randIndex] ].m_tileCoords.x, (float)m_tiles[ availableTilesIndex[randIndex] ].m_tileCoords.y );
	Vector2 spawnPosition = tilePosition + Vector2( 0.5f, 0.5f );
	if( actor != nullptr )
		actor->m_position = spawnPosition;
	else
		m_currentPlayer.m_position = spawnPosition;
}

void Map::CreatePortal( std::string portalType, std::string onTileType, std::string destinationMap, std::string reciprocalPortalType )
{
	// Get vector of index of all tiles whose definition is onTileType
	std::vector< int > availableTilesIndex;
	for( int i = 0; i < (m_dimension.x * m_dimension.y) - 1.f; i++ )
	{
		if( m_tiles[i].m_tileDef->m_tileName == onTileType && m_tiles[i].m_tileExtraInfo->m_tags.HasTags("entitySpawned") == false  )
		{
			availableTilesIndex.push_back( i );
		}
	}

	// Choose a random index
	int randIndex = GetRandomIntInRange( 0, availableTilesIndex.size() - 1 );
	// Spawn a Portal
	Vector2 tilePosition = Vector2( (float)m_tiles[ availableTilesIndex[randIndex] ].m_tileCoords.x, (float)m_tiles[ availableTilesIndex[randIndex] ].m_tileCoords.y );
	Vector2 spawnPosition = tilePosition + Vector2( 0.5f, 0.5f );
	Portal* newPortal = new Portal( spawnPosition, portalType, destinationMap, reciprocalPortalType );
	AddEntityToList( newPortal, ENTITY_TYPE_PORTAL );

	// Add Tag to tile that "actorSpawned"
	m_tiles[ availableTilesIndex[randIndex] ].m_tileExtraInfo->m_tags.SetOrRemoveTags("entitySpawned");
}

std::vector< Portal* > Map::GetAllPortals()
{
	std::vector< Portal* > allPortals;

	for( Entity* thisPortal : m_entities[ ENTITY_TYPE_PORTAL ] )
	{
		allPortals.push_back( static_cast< Portal* >( thisPortal ) );
	}

	return allPortals;
}

void Map::CheckForTileVsEntityCollosion() {

	EntityType entityTypesToCheck[] = {
		ENTITY_TYPE_PLAYER,
		ENTITY_TYPE_NP_ACTOR
	};

	// For EntityList(s) present in provided enum
	for( EntityType currentType : entityTypesToCheck )
	{
		// Do collision check for each entities present in that type
		for( Entity* currentEntity : m_entities[ currentType ] )
		{
			IntVector2 targetTileCoords = IntVector2( (int)currentEntity->m_position.x , (int)currentEntity->m_position.y) + STEP_NE;
			int index = GetIndexOfTileAt( targetTileCoords );
			
			Tile tileToTest = m_tiles[index];
			CorrectEntitysPosition( *currentEntity, tileToTest );

			targetTileCoords = IntVector2( (int)currentEntity->m_position.x , (int)currentEntity->m_position.y) + STEP_NW;
			index = GetIndexOfTileAt( targetTileCoords );
			tileToTest = m_tiles[index];
			CorrectEntitysPosition( *currentEntity, tileToTest );

			targetTileCoords = IntVector2( (int)currentEntity->m_position.x , (int)currentEntity->m_position.y) + STEP_SE;
			index = GetIndexOfTileAt( targetTileCoords );
			tileToTest = m_tiles[index];
			CorrectEntitysPosition( *currentEntity, tileToTest );

			targetTileCoords = IntVector2( (int)currentEntity->m_position.x , (int)currentEntity->m_position.y) + STEP_SW;
			index = GetIndexOfTileAt( targetTileCoords );
			tileToTest = m_tiles[index];
			CorrectEntitysPosition( *currentEntity, tileToTest );


			targetTileCoords = IntVector2( (int)currentEntity->m_position.x , (int)currentEntity->m_position.y) + STEP_EAST;
			index = GetIndexOfTileAt( targetTileCoords );
			tileToTest = m_tiles[index];
			CorrectEntitysPosition( *currentEntity, tileToTest );

			targetTileCoords = IntVector2( (int)currentEntity->m_position.x , (int)currentEntity->m_position.y) + STEP_WEST;
			index = GetIndexOfTileAt( targetTileCoords );
			tileToTest = m_tiles[index];
			CorrectEntitysPosition( *currentEntity, tileToTest );

			targetTileCoords = IntVector2( (int)currentEntity->m_position.x , (int)currentEntity->m_position.y) + STEP_NORTH;
			index = GetIndexOfTileAt( targetTileCoords );
			tileToTest = m_tiles[index];
			CorrectEntitysPosition( *currentEntity, tileToTest );

			targetTileCoords = IntVector2( (int)currentEntity->m_position.x , (int)currentEntity->m_position.y) + STEP_SOUTH;
			index = GetIndexOfTileAt( targetTileCoords );
			tileToTest = m_tiles[index];
			CorrectEntitysPosition( *currentEntity, tileToTest );
		}
	}
}

void Map::UpdateProjectileWithCollision( float deltaSeconds )
{
	EntityType entityTypesToCheck[] = {
		ENTITY_TYPE_PROJECTILE
	};

	for( EntityType& eType : entityTypesToCheck ) {
		EntityList& projectileList = m_entities[ eType ];

		for( unsigned int projectileIndex = 0; projectileIndex < projectileList.size(); projectileIndex++ )
		{
			// FOR EVERY PROJECTILE
			Projectile* thisProjectile = (Projectile*) projectileList[ projectileIndex ];

			// Current & Next Position
			Vector2 currPosition = thisProjectile->m_position;
			Vector2 nextPosition = thisProjectile->m_position + ( thisProjectile->m_velocity * deltaSeconds );

			// Next Tile coordinates
			IntVector2 nextTileCoord = IntVector2( (int)nextPosition.x , (int)nextPosition.y );
			IntVector2 currTileCoord = IntVector2( (int)currPosition.x , (int)currPosition.y );

			// If next tile doesn't allow walking
			int indexOfNextTile = GetIndexOfTileAt(nextTileCoord);
			if( m_tiles[indexOfNextTile].m_tileDef->m_allowsWalking == false ) 
			{
				thisProjectile->m_health--;

				// Change Vector
				if( currTileCoord.x != nextTileCoord.x ) {
					// flip x
					thisProjectile->m_velocity = Vector2( thisProjectile->m_velocity.x * -1.f , thisProjectile->m_velocity.y );
				}

				if( currTileCoord.y != nextTileCoord.y ) {
					// flip y
					thisProjectile->m_velocity = Vector2( thisProjectile->m_velocity.x , thisProjectile->m_velocity.y * -1.f );
				}
				thisProjectile->m_orientationAngleInDegree = thisProjectile->m_velocity.GetOrientationDegrees();
			}
			// If next tile is not solid, then move
			else
			{
				thisProjectile->Update(deltaSeconds);
			}
		}
	}
}

void Map::CorrectEntitysPosition( Entity& thisEntity, Tile& tile ) {
	if( tile.m_tileDef->m_allowsWalking == true) {		// Doing collision just for tiles which do not allow walking
		return;
	}

	Vector2 closestPoint = tile.GetClosestPointOnTileFrom( thisEntity.m_position );
	Disc2	collisionDisc_thiEntity = thisEntity.GetCollisionDisc();

	if( Disc2::DoDiscAndPointOverlap( collisionDisc_thiEntity, closestPoint) ) {
		// Push out..
		Vector2 pushDirection = thisEntity.m_position - closestPoint;
		pushDirection.NormalizeAndGetLength();

		float pushDistance = collisionDisc_thiEntity.radius - Vector2::GetDistance( closestPoint , thisEntity.m_position );

		Vector2 newPosition = thisEntity.m_position + ( pushDirection * pushDistance );
		thisEntity.m_position = newPosition;
	}
	else
	{
		return;
	}

}

void Map::CheckForProjectileVsActorCollision()
{
	EntityType actorsToCheck[] = {
		ENTITY_TYPE_PLAYER,
		ENTITY_TYPE_NP_ACTOR
	};

	// Check collision with every projectiles
	for( Entity* thisProjectile : m_entities[ ENTITY_TYPE_PROJECTILE ] )
	{
		// For every actorType in the checkList
		for( EntityType currentType : actorsToCheck )
		{
			// For every actors in that type's EntityList
			for( Entity* currentActor : m_entities[ currentType ] )
			{
				// Check for collision
				bool theyDoOverlap = Disc2::DoDiscAndPointOverlap( currentActor->GetCollisionDisc() , thisProjectile->m_position );
				if( theyDoOverlap )
				{
					// If factions doesn't match
					if( static_cast< ActorDefinition* >(currentActor->m_definition)->m_faction != static_cast< Projectile* >(thisProjectile)->m_faction )
					{
						// Reduce Projectile's health
						static_cast< Projectile* >(thisProjectile)->m_health --;
						// Reduce actor's health
						static_cast< Actor* >(currentActor)->m_currentHealth -= static_cast< ProjectileDefinition* >(thisProjectile->m_definition)->m_damageAmount;
					}
				}
			}
		}
	}
}

void Map::CheckForPortalVsActorCollision()
{
	EntityType actorsToCheck[] = {
		ENTITY_TYPE_PLAYER
	};

	// Check collision with every portal
	for( Entity* thisPortal : m_entities[ ENTITY_TYPE_PORTAL ] )
	{
		// If portal is disabled, skip..
		if( static_cast<Portal*>(thisPortal)->m_isDisabled )
			continue;

		// For every actorType in the checkList
		for( EntityType currentType : actorsToCheck )
		{
			// For every actors in that type's EntityList
			for( Entity* currentActor : m_entities[ currentType ] )
			{
				// Check for collision
				bool theyDoOverlap = Disc2::DoDiscAndPointOverlap( currentActor->GetCollisionDisc() , thisPortal->m_position );
				if( theyDoOverlap )
				{
					Portal* thisPortal_casted = static_cast< Portal* >(thisPortal);
					g_theGame->m_currentAdventure->SetAsCurrentMap( thisPortal_casted->m_destinationMapName );
					currentActor->m_position = thisPortal_casted->m_reciprocalPortal->PrepPortalForPlayerSpawn();
				}
			}
		}
	}
}

void Map::CheckForActorVsActorCollision()
{
	EntityType actorsToCheck[] = {
		ENTITY_TYPE_PLAYER,
		ENTITY_TYPE_NP_ACTOR
	};
	
	// Create one combined list..
	EntityList allActorsCombined;
	for( EntityType eType : actorsToCheck )
	{
		allActorsCombined.insert( allActorsCombined.begin(), m_entities[ eType ].begin(), m_entities[ eType ].end() );
	}

	// Run for loop to check collision on it
	for( unsigned int i=0; i<allActorsCombined.size(); i++ )
	{
		for( unsigned int j=i+1; j<allActorsCombined.size(); j++ )
		{
			// Check for collision
			Disc2 iDisc = allActorsCombined[i]->GetCollisionDisc();
			Disc2 jDisc = allActorsCombined[j]->GetCollisionDisc();
			if( Disc2::DoDiscsOverlap( iDisc , jDisc ) )
			{
				float totalPushBack = iDisc.radius + jDisc.radius - Vector2::GetDistance( iDisc.center , jDisc.center );

				float otherEnemyPushBack = totalPushBack / 2.f;
				float thisEnemyPushBack = totalPushBack / 2.f;

				Vector2 thisEnemyPushBackDirection = iDisc.center - jDisc.center;
				thisEnemyPushBackDirection.NormalizeAndGetLength();
				allActorsCombined[i]->m_position += (thisEnemyPushBackDirection * thisEnemyPushBack);

				Vector2 otherEnemyPushBackDirection = jDisc.center - iDisc.center;
				otherEnemyPushBackDirection.NormalizeAndGetLength();
				allActorsCombined[j]->m_position += (otherEnemyPushBackDirection * otherEnemyPushBack);
			}
		}
	}
}

// TODO: Its maxDistance for Goblin -> Player is not correct, sort out the issue
RaycastResult Map::Raycast( const Vector2& startPos, const Vector2& direction, float maxDistance ) const {
	// Setting up default return variables
	Vector2 impactPosition = Vector2( 0.f, 0.f );
	IntVector2 currentTileCoords = IntVector2( 0, 0 );
	Vector2 impactNormal = Vector2( -1.f, 0.f );
	float fractionTravelled = 1.1f;
	// Default result in case of no impact..
	RaycastResult result = RaycastResult( false, impactPosition, currentTileCoords, fractionTravelled, impactNormal );


	float numSteps = maxDistance * RAYCAST_STEPS_PER_TILE;
	Vector2 singleStep = direction / numSteps;
	IntVector2 previousTileCoords = IntVector2( (int)startPos.x , (int)startPos.y );

	int index = GetIndexOfTileAt(previousTileCoords);
	// If start position is in solid tile
	if( m_tiles[index].m_tileDef->m_allowsSight == false ) {
		impactPosition = startPos;
		currentTileCoords = previousTileCoords;
		impactNormal = Vector2( -1.f , 0.f );
		fractionTravelled = 0.f;
		return RaycastResult( true, impactPosition, currentTileCoords, fractionTravelled, impactNormal);
	}

	// For every step, until the end
	for( float currStep = 1.f; currStep <= numSteps; currStep++ ) {
		Vector2 currentPosition = startPos + (singleStep * currStep);
		currentTileCoords = IntVector2( (int)currentPosition.x , (int)currentPosition.y );

		// If currentTile is same previousTile, no need to check
		// If it is different, only then check for RayCast
		if( currentTileCoords != previousTileCoords ) {

			index = GetIndexOfTileAt(currentTileCoords);
			// If this is a solid tile
			if( m_tiles[index].m_tileDef->m_allowsSight == false ) {
				impactPosition = currentPosition;
				fractionTravelled = Vector2::GetDistance(startPos, currentPosition) / maxDistance;
				impactNormal = Vector2( (float)previousTileCoords.x - (float)currentTileCoords.x , (float)previousTileCoords.y - (float)currentTileCoords.y ).GetNormalized();
				return RaycastResult( true, impactPosition, currentTileCoords, fractionTravelled, impactNormal);
			}
		}

		previousTileCoords = currentTileCoords;
	}

	// If no impact, return default RaycastResult
	return result;
}

bool Map::HasLineOfSight( const Vector2& startPos, const Vector2& endPos ) const {
	Vector2 direction = (endPos - startPos);
	float maxDistance = direction.NormalizeAndGetLength();

	RaycastResult result = Raycast( startPos , direction, maxDistance );

	return !result.m_didImpact;
}


int Map::GetIndexOfTileAt( IntVector2 tileCoords ) const {
	int columnNum = tileCoords.x;
	int rowNum = tileCoords.y;

	return ( rowNum * m_dimension.x ) + columnNum;
}

void Map::RenderAllPortals()
{
	for( Entity* portal : m_entities[ ENTITY_TYPE_PORTAL ] )
	{
		portal->Render();
	}
}

void Map::SortAndRenderAllEntities()
{
	for( std::vector< Entity* >::iterator it = m_allEntities.begin(); it != m_allEntities.end() && it+1 != m_allEntities.end(); it++ )
	{
		float y_E1 = (*it)->m_position.y;
		float y_E2 = (*(it+1))->m_position.y;

		if( y_E1 < y_E2 )
		{
			// swap that two..
			Entity* tmp	= *it;
			*it			= *(it+1);
			*(it+1)		= tmp;
		}
	}

	for( Entity* entity : m_allEntities )
	{
		std::string entityName = entity->m_definition->m_name;
		if( GetEntityTypeFromDefinitionName(entityName) != ENTITY_TYPE_PORTAL )			// We do not want to draw Portals, in sorted way; Portals should always be at bottom
			entity->Render();
	}
}

void Map::DeleteEntitiesReadyToDie()
{
	EntityType typesToDelete[] = {
		ENTITY_TYPE_NP_ACTOR,
		ENTITY_TYPE_PROJECTILE
	};

	// Remove from specific entity lists
	for( EntityType currentType : typesToDelete )
	{
		EntityList& currentList = m_entities[ currentType ];
		for( unsigned int index = 0; index < currentList.size(); index++ )
		{
			Entity* currentEntity = currentList.at( index );
			if( currentEntity->m_readyToDie == true )
			{
				// Erase the Entity from m_allEntities
				EntityList::iterator it = std::find( m_allEntities.begin(), m_allEntities.end(), currentEntity );
				m_allEntities.erase( it );

				// delete the Entity and set it to nullptr
				delete currentEntity;
				currentEntity = nullptr;

				// Erase the Entity from currentList, as well
				currentList.erase( currentList.begin() + index );
			}
		}
	}
}