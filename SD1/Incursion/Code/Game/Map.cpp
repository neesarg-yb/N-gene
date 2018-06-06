#include "Map.hpp"
#include "Game/GameCommon.hpp"

RaycastResult::RaycastResult( bool didImpact, const Vector2& impactPosition, const IntVector2& impactTileCoords, float fractionTravelled, const Vector2& impactNormal ) {
	m_didImpact				= didImpact;
	m_impactPosition		= impactPosition;
	m_impactTileCoords		= impactTileCoords;
	m_atFractionTravelled	= fractionTravelled;
	m_impactNormal			= impactNormal;
}

Map::Map( const IntVector2& gridSize, PlayerTank* cPlayer  ) {

	// Set gridSize & reference to the player
	this->gridSize = gridSize;
	AddEntityToList( cPlayer , ENTITY_TYPE_PLAYER_TANK );
	theTank = cPlayer;
	
	// Setup TileDefinations
	TileDefination::s_TileDefinations[ TILE_TYPE_GRASS ] = TileDefination( TILE_TYPE_GRASS, false, IntVector2(1,1) );
	TileDefination::s_TileDefinations[ TILE_TYPE_STONE ] = TileDefination( TILE_TYPE_STONE,  true, IntVector2(3,3) );

	// Generate Map with Tiles
	PopulateMapWithTiles();

	// Setup theSpriteSheet
	spriteSheetTexture = g_theRenderer->CreateOrGetTexture(pathToTexturePNG);
	theSpriteSheet = new SpriteSheet( *spriteSheetTexture , 8 , 8);

	// TESTING ENEMY_TURRET
	for(int i=0; i<10; i++) {
		Entity* tempTurr = SpawnNewEntity( ENTITY_TYPE_ENEMY_TURRET , Vector2( 10.5f, 10.5f) , (float) MathUtil::GetRandomNonNegativeIntLessThan(360) );
		AddEntityToList( tempTurr , ENTITY_TYPE_ENEMY_TURRET );
	}

	// TESTING ENEMY_TANK
	for(int i=0; i<10; i++) {
		Entity* tempTurr = SpawnNewEntity( ENTITY_TYPE_ENEMY_TANK , Vector2( 10.5f, 10.5f) , (float) MathUtil::GetRandomNonNegativeIntLessThan(360) );
		AddEntityToList( tempTurr , ENTITY_TYPE_ENEMY_TANK );
	}

	// Spawn Entities Randomly
	PlaceAllEntitiesRandomly();

	// Load sounds
	enemyHitSound = g_theAudioSystem->CreateOrGetSound(enemyHitSoundPath);
	enemyDiedSound = g_theAudioSystem->CreateOrGetSound(enemyDiedSoundPath);
	playerHitSound = g_theAudioSystem->CreateOrGetSound(playerHitSoundPath);
	playerDiedSound = g_theAudioSystem->CreateOrGetSound(playerDiedSoundPath);
}

Map::~Map()
{
	delete theSpriteSheet;
	//delete spriteSheetTexture;
}

void Map::BeginFrame() {

}

void Map::EndFrame() {
	// DELETE ALL markForDelete ENTITIES
	for( int entityType = 0; entityType < NUM_ENTITY_TYPES; entityType++ ) 
	{
		EntityList& thisVector = m_entities[ entityType ];
		for( unsigned int i = 0; i < thisVector.size(); i++ ) 
		{
			Entity*& entity = thisVector[i];
			if( entity->m_markForDelete ) 
			{
				delete entity;
				thisVector.erase( thisVector.begin() + i );
			}
		}
	}
}

void Map::Update( float deltaSeconds ) {
	// Time playerIsInvincible
	if( playerIsInvincible )
	{
		static float invincibleTimeElasped = 0.f;
		float maxInvincibleTime = 3.f;

		if( invincibleTimeElasped >= maxInvincibleTime )
		{
			playerIsInvincible = false;
			invincibleTimeElasped = 0.f;
		}

		invincibleTimeElasped += deltaSeconds;
	}

	if( g_theInput->m_controller[0].m_xboxTriggerStates[ XBOX_TRIGGER_RIGHT ] > 0.5f ) {
		theTank->currentOverheatInTimeUnits = MathUtil::ClampFloat( theTank->currentOverheatInTimeUnits + deltaSeconds , 0 , theTank->maxOverheatAfterTime );

		PlayerBullet* newBullet = theTank->createABullet( deltaSeconds );
		if( newBullet != nullptr) {
			AddEntityToList( newBullet , ENTITY_TYPE_PLAYER_BULLET );
		}
	}
	else
	{
		theTank->currentOverheatInTimeUnits = MathUtil::ClampFloat( theTank->currentOverheatInTimeUnits - deltaSeconds , 0 , theTank->maxOverheatAfterTime );
	}


	// UPDATE ALL ENTITIES
	for( int entityType = 0; entityType < NUM_ENTITY_TYPES; entityType++ ) 
	{
		if( entityType != ENTITY_TYPE_PLAYER_BULLET && entityType != ENTITY_TYPE_ENEMY_BULLET) {
			for( Entity*& entity : m_entities[ entityType ] ) 
			{
				entity->Update( deltaSeconds );
			}
		}
	}
	

	// CHECK FOR COLLISIONS
	if( !zeroModeEnable )
		CheckPlayerTankVsEnemiesCollision();
	
	CheckForEntityVsTileCollosion();
	CheckPlayerBulletsVsEnemiesCollision();
	
	if( !zeroModeEnable && !playerIsInvincible )
		CheckEnemyBulletsVsPlayerTankCollision();
	
	CheckEnemiesVsEnemiesCollision();
	PreventBulletCollisions(deltaSeconds);
}

void Map::Render() {


	// RENDER ALL TILES
	// for every rows
	for(int i=0; i<gridSize.x; i++) 
	{
		// for every columns
		for(int j=0; j<gridSize.y; j++) 
		{
			DrawTileAt( i, j );
		}
	}


	// RENDER ALL ENTITIES
	for( int entityType = 0; entityType < NUM_ENTITY_TYPES; entityType++ ) 
	{
		for( Entity*& entity : m_entities[ entityType ] ) 
		{
			entity->Render();

			if( debugModeOn )
				entity->drawDebugInformations();
		}
	}
}

void Map::PopulateMapWithTiles() {
	// If tile is on boundary, make it stone
	//				 otherwise, make it grass
	Tile tileToPush;

	// for every rows
	for(int i=0; i<gridSize.x; i++) {

		// for every columns
		for(int j=0; j<gridSize.y; j++) {
			tileToPush.m_tileCoords = IntVector2( i, j );

			// if tile is on boundary, make it stone
			if( i == 0 || i == gridSize.x-1 || j == 0 || j == gridSize.y-1 ) {
				tileToPush.m_tileType = TILE_TYPE_STONE;
				m_tiles.push_back( tileToPush );

			} else {
				// else make 10% of tiles STONE_TYPE
				tileToPush.m_tileType = TILE_TYPE_GRASS;
				if( MathUtil::CheckRandomChance(0.10f) ) {
					tileToPush.m_tileType = TILE_TYPE_STONE;
				}
				m_tiles.push_back( tileToPush );
			}
		}
	}
}

void Map::DrawTileAt( int row, int column ) {
	Vector2 mins = Vector2( (float)row, (float)column );
	Vector2 maxs = Vector2( (float)row+1, (float)column+1 );
	AABB2 boundsOfTile = AABB2(mins, maxs);

	int index = GetIndexOfTileAt(row, column);
	TileType tileType = m_tiles[index].m_tileType;
	
	IntVector2 textCoords = TileDefination::s_TileDefinations[ tileType ].m_tileCoords;
	AABB2 textCoordsBox = theSpriteSheet->GetTexCoordsForSpriteCoords(textCoords);
	g_theRenderer->DrawTexturedAABB(boundsOfTile, *spriteSheetTexture, textCoordsBox.mins, textCoordsBox.maxs, RGBA_WHITE_COLOR );
}

int Map::GetIndexOfTileAt( int row, int column ) const {
	row ++;
	column ++;

	return ( (gridSize.y * (row-1)) + column ) - 1;
}

int Map::GetIndexOfTileAt( IntVector2 tileCoords ) const {
	int row = tileCoords.x + 1;
	int column = tileCoords.y + 1;

	return ( (gridSize.y * (row-1)) + column ) - 1;
}

void Map::RespawnPlayerTank()
{
	playerIsInvincible = true;

	theTank->m_health = 10.0f;
	theTank->isInvisible = false;
}

void Map::CheckForEntityVsTileCollosion() {
	EntityType entityTypesToCheck[] = {
		ENTITY_TYPE_PLAYER_TANK,
		ENTITY_TYPE_ENEMY_TANK,
		ENTITY_TYPE_ENEMY_TURRET
	};

	for( EntityType& eType : entityTypesToCheck ) {
		// ALL eType-TO-TILE COLLISIONS
		for( Entity* currentEntity : m_entities[eType] ) {

			IntVector2 targetTileCoords = IntVector2( (int)currentEntity->GetPosition().x , (int)currentEntity->GetPosition().y) + STEP_NE;
			int index = GetIndexOfTileAt( targetTileCoords );
			Tile tileToTest = m_tiles[index];
			CorrectEntitysPosition( *currentEntity, tileToTest );

			targetTileCoords = IntVector2( (int)currentEntity->GetPosition().x , (int)currentEntity->GetPosition().y) + STEP_NW;
			index = GetIndexOfTileAt( targetTileCoords );
			tileToTest = m_tiles[index];
			CorrectEntitysPosition( *currentEntity, tileToTest );

			targetTileCoords = IntVector2( (int)currentEntity->GetPosition().x , (int)currentEntity->GetPosition().y) + STEP_SE;
			index = GetIndexOfTileAt( targetTileCoords );
			tileToTest = m_tiles[index];
			CorrectEntitysPosition( *currentEntity, tileToTest );

			targetTileCoords = IntVector2( (int)currentEntity->GetPosition().x , (int)currentEntity->GetPosition().y) + STEP_SW;
			index = GetIndexOfTileAt( targetTileCoords );
			tileToTest = m_tiles[index];
			CorrectEntitysPosition( *currentEntity, tileToTest );


			targetTileCoords = IntVector2( (int)currentEntity->GetPosition().x , (int)currentEntity->GetPosition().y) + STEP_EAST;
			index = GetIndexOfTileAt( targetTileCoords );
			tileToTest = m_tiles[index];
			CorrectEntitysPosition( *currentEntity, tileToTest );

			targetTileCoords = IntVector2( (int)currentEntity->GetPosition().x , (int)currentEntity->GetPosition().y) + STEP_WEST;
			index = GetIndexOfTileAt( targetTileCoords );
			tileToTest = m_tiles[index];
			CorrectEntitysPosition( *currentEntity, tileToTest );

			targetTileCoords = IntVector2( (int)currentEntity->GetPosition().x , (int)currentEntity->GetPosition().y) + STEP_NORTH;
			index = GetIndexOfTileAt( targetTileCoords );
			tileToTest = m_tiles[index];
			CorrectEntitysPosition( *currentEntity, tileToTest );

			targetTileCoords = IntVector2( (int)currentEntity->GetPosition().x , (int)currentEntity->GetPosition().y) + STEP_SOUTH;
			index = GetIndexOfTileAt( targetTileCoords );
			tileToTest = m_tiles[index];
			CorrectEntitysPosition( *currentEntity, tileToTest );
		}
	}	
}

void Map::CorrectEntitysPosition( Entity& thisEntity, Tile& tile ) {
	if( tile.m_tileType != TILE_TYPE_STONE) {		// Doing collision just for grass
		return;
	}

	AABB2 boundsOfTile = tile.GetBoundsOfTile();
	Vector2 centerOfTile = tile.GetCenterOfTile();
	Vector2 closestPoint = tile.GetClosestPointOnTileFrom( thisEntity.GetPosition() );

	if( Disc2::DoDiscAndPointOverlap( thisEntity.m_collosionDisc2, closestPoint) ) {
		// Push out..
		Vector2 pushDirection = thisEntity.GetPosition() - closestPoint;
		pushDirection.NormalizeAndGetLength();

		float pushDistance = thisEntity.m_collosionDisc2.radius - Vector2::GetDistance( closestPoint , thisEntity.GetPosition() );

		Vector2 newPosition = thisEntity.GetPosition() + pushDirection * pushDistance;
		thisEntity.SetPosition( newPosition );
	}
	else
	{
		return;
	}

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
	}
}

RaycastResult Map::Raycast( const Vector2& startPos, const Vector2& direction, float maxDistance ) const {
	// Setting up default return variables
	Vector2 impactPosition = Vector2( 0.f, 0.f );
	IntVector2 currentTileCoords = IntVector2( 0, 0 );
	Vector2 impactNormal = Vector2( -1.f, 0.f );
	float fractionTravelled = 1.1f;
	// Default result in case of no impact..
	RaycastResult result = RaycastResult( false, impactPosition, currentTileCoords, fractionTravelled, impactNormal );


	float numSteps = maxDistance * RAYCAST_STEPS_PER_TILE;
	Vector2 singleStep = direction / RAYCAST_STEPS_PER_TILE;
	IntVector2 previousTileCoords = IntVector2( (int)startPos.x , (int)startPos.y );
	
	int index = GetIndexOfTileAt(previousTileCoords);
	// If start position is in solid tile
	if( m_tiles[index].m_tileType == TILE_TYPE_STONE ) {
		impactPosition = startPos;
		currentTileCoords = previousTileCoords;
		impactNormal = Vector2( -1.f , 0.f );
		fractionTravelled = 0.f;
		return RaycastResult( true, impactPosition, currentTileCoords, fractionTravelled, impactNormal);
	}

	// For every step, untill the end
	for( float currStep = 1.f; currStep <= numSteps; currStep++ ) {
		Vector2 currentPosition = startPos + (singleStep * currStep);
		currentTileCoords = IntVector2( (int)currentPosition.x , (int)currentPosition.y );

		// If currentTile is same previousTile, no need to check
		// If it is diffrent, only then check for RayCast
		if( currentTileCoords != previousTileCoords ) {

			index = GetIndexOfTileAt(currentTileCoords);
			// If this is a solid tile
			if( m_tiles[index].m_tileType == TILE_TYPE_STONE ) {
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

void Map::PlaceAllEntitiesRandomly() {
	EntityType entityTypesToSpawnRandomly[] = {
		ENTITY_TYPE_PLAYER_TANK,
		ENTITY_TYPE_ENEMY_TANK,
		ENTITY_TYPE_ENEMY_TURRET
	};

	int maxIndexOfTiles = (int) m_tiles.size();

	for( EntityType& type : entityTypesToSpawnRandomly ) {
		EntityList& entityToSpawn = m_entities[ type ];

		for( unsigned int entityIndex = 0; entityIndex < entityToSpawn.size(); entityIndex++ )
		{
			// Get random for index of tile
			int spawnIndex = MathUtil::GetRandomNonNegativeIntLessThan( maxIndexOfTiles );		// Argument is non-inclusive

																								// check if that tile is not-solid
			Tile& spawnAtTile = m_tiles[ spawnIndex ];
			if( spawnAtTile.m_tileType == TILE_TYPE_GRASS )
			{
				// spawn the damn thing
				Vector2 tileCenter = Vector2( spawnAtTile.m_tileCoords.x + 0.5f , spawnAtTile.m_tileCoords.y + 0.5f );
				entityToSpawn[ entityIndex ]->SetPosition( tileCenter );
			}
			else {
				// else if tile is solid
				entityIndex--;
			}
		}
	}	
}

Entity* Map::SpawnNewEntity( EntityType type, Vector2 position, float orientation ) {
	Entity* toSpawn = nullptr;

	switch (type)
	{
	case ENTITY_TYPE_PLAYER_TANK:
		toSpawn = new PlayerTank( position , orientation );
		break;

	case ENTITY_TYPE_ENEMY_TANK:
		toSpawn = new EnemyTank( position , orientation );
		break;
	
	case ENTITY_TYPE_ENEMY_TURRET:
		toSpawn = new EnemyTurret( position , orientation );
		break;

	default:
		toSpawn = nullptr;
		break;
	}

	return toSpawn;
}

void Map::PreventBulletCollisions( float deltaSeconds ) {
	EntityType entityTypesToCheck[] = {
		ENTITY_TYPE_PLAYER_BULLET,
		ENTITY_TYPE_ENEMY_BULLET
	};

	for( EntityType& eType : entityTypesToCheck ) {
		EntityList& bulletList = m_entities[ eType ];

		for( unsigned int bulletIndex = 0; bulletIndex < bulletList.size(); bulletIndex++ )
		{
			// FOR EVERY BULLETS
			Entity* thisBullet = bulletList[ bulletIndex ];

			// Current & Next Position
			Vector2 currPosition = thisBullet->GetPosition();
			Vector2 nextPosition = thisBullet->GetPosition() + ( thisBullet->m_velocity * deltaSeconds );

			// Next Tile coordinates
			IntVector2 nextTileCoord = IntVector2( (int)nextPosition.x , (int)nextPosition.y );
			IntVector2 currTileCoord = IntVector2( (int)currPosition.x , (int)currPosition.y );

			// If next tile is solid
			int indexOfNextTile = GetIndexOfTileAt(nextTileCoord);
			if( m_tiles[indexOfNextTile].m_tileType == TILE_TYPE_STONE ) 
			{
				thisBullet->ReduceAHealth();

				// Change Vector
				if( currTileCoord.x != nextTileCoord.x ) {
					// flip x
					thisBullet->m_velocity = Vector2( thisBullet->m_velocity.x * -1.f , thisBullet->m_velocity.y );
				}

				if( currTileCoord.y != nextTileCoord.y ) {
					// flip y
					thisBullet->m_velocity = Vector2( thisBullet->m_velocity.x , thisBullet->m_velocity.y * -1.f );
				}
				thisBullet->m_orientationAngleInDegree = thisBullet->m_velocity.GetOrientationDegrees();
			}
			// If next tile is not solid, then move
			else
			{
				thisBullet->Update(deltaSeconds);
			}
		}
	}
}

void Map::CheckPlayerBulletsVsEnemiesCollision()
{
	EntityType enemiesToCheck[] = {
		ENTITY_TYPE_ENEMY_TURRET,
		ENTITY_TYPE_ENEMY_TANK
	};

	// For every enemy type
	for( EntityType& eType : enemiesToCheck ) {
		EntityList& enemyList = m_entities[ eType ];

		// For every enemy of that type
		for( unsigned int enemyIndex = 0; enemyIndex < enemyList.size(); enemyIndex++ )
		{
			Entity* thisEnemy = enemyList[ enemyIndex ];

			// For every PlayerBullets
			EntityList& playerBullets = m_entities[ ENTITY_TYPE_PLAYER_BULLET ];
			for( unsigned int bulletIndex = 0; bulletIndex < playerBullets.size(); bulletIndex++ )
			{
				Entity* thisBullet = playerBullets[ bulletIndex ];

				// Check for collision
				bool theyDoOverlap = Disc2::DoDiscAndPointOverlap( thisEnemy->m_collosionDisc2 , thisBullet->GetPosition() );
				if( theyDoOverlap )
				{
					// Reduce PlayerBullet's health by three...
					thisBullet->ReduceAHealth();
					thisBullet->ReduceAHealth();
					thisBullet->ReduceAHealth();

					// Reduce enemy's health by one...
					thisEnemy->ReduceAHealth();

					// Setup the explosion
					Vector2 explosionPosition = thisBullet->GetPosition();
					float explosionRadius = 0.2f;
					float explosionDuration = 1.5f;

					// If normal hit, set hit sound to play
					SoundID idToPlay = enemyHitSound;
					float soundVolume = 0.5f;

					if( thisEnemy->m_health <= 0 )
					{
						// If enemy dies, set died sound to play
						idToPlay = enemyDiedSound;
						soundVolume = 1.0f;

						explosionPosition = thisEnemy->GetPosition();
						explosionRadius = eType == ENTITY_TYPE_ENEMY_TURRET ? 0.8f : 0.7f;
						explosionDuration = 3.0f;
					}

					// Create Explosion
					Explosion* newExplosion = new Explosion( explosionPosition , explosionRadius, explosionDuration );
					AddEntityToList(newExplosion, ENTITY_TYPE_EXPLOSION);

					// Create Explosion sound..
					g_theAudioSystem->PlaySound(idToPlay, false, soundVolume, 0.0f, 1.0f, false);
				}
			}
		}
	}
}

void Map::CheckEnemyBulletsVsPlayerTankCollision()
{
	EntityList& playerTankList = m_entities[ ENTITY_TYPE_PLAYER_TANK ];

	// For every playerTanks
	for( unsigned int playerTankIndex = 0; playerTankIndex < playerTankList.size(); playerTankIndex++ )
	{
		PlayerTank* thisTank = (PlayerTank*) playerTankList[ playerTankIndex ];

		// For every EnemyBullets
		EntityList& enemyBullets = m_entities[ ENTITY_TYPE_ENEMY_BULLET ];
		for( unsigned int bulletIndex = 0; bulletIndex < enemyBullets.size() && thisTank->isInvisible == false; bulletIndex++ )
		{
			Entity* thisBullet = enemyBullets[ bulletIndex ];

			// Check for collision
			bool theyDoOverlap = Disc2::DoDiscAndPointOverlap( thisTank->m_collosionDisc2 , thisBullet->GetPosition() );
			if( theyDoOverlap )
			{
				// Reduce PlayerBullet's health by three...
				thisBullet->ReduceAHealth();

				// Reduce enemy's health by one...
				thisTank->TakeOneHitFromBullet();				// Using this function, coz I don't want tank to be deleted. I'm just making it invisible..


				// Setup the explosion
				Vector2 explosionPosition = thisBullet->GetPosition();
				float explosionRadius = 0.2f;
				float explosionDuration = 1.5f;

				// If normal hit, set hit sound to play
				SoundID idToPlay = playerHitSound;
				float soundVolume = 0.5f;

				if( thisTank->m_health <= 0 )
				{
					// If player dies, set died sound to play
					idToPlay = playerDiedSound;
					soundVolume = 1.0f;

					explosionPosition = thisTank->GetPosition();
					explosionRadius = 0.9f;
					explosionDuration = 1.0f;		// Because of slowMo ON when player dies, we need to make explosionDuration smaller than expected
				}

				// Create Explosion
				Explosion* newExplosion = new Explosion( explosionPosition , explosionRadius, explosionDuration );
				AddEntityToList(newExplosion, ENTITY_TYPE_EXPLOSION);

				// Create Explosion sound..
				g_theAudioSystem->PlaySound(idToPlay, false, soundVolume, 0.0f, 1.0f, false);

				// Enable screenshake
				g_theGame->screenShakeEnabled = true;
			}
		}
	}
}

void Map::CheckPlayerTankVsEnemiesCollision()
{
	EntityType enemiesToCheck[] = {
		ENTITY_TYPE_ENEMY_TURRET,
		ENTITY_TYPE_ENEMY_TANK
	};

	EntityList& playerTankList = m_entities[ ENTITY_TYPE_PLAYER_TANK ];

	// For every playerTanks
	for( unsigned int playerTankIndex = 0; playerTankIndex < playerTankList.size(); playerTankIndex++ )
	{
		PlayerTank* thisTank = (PlayerTank*) playerTankList[ playerTankIndex ];
		if( thisTank->isInvisible )
		{
			return;
		}

		// For every enemy types
		for( EntityType& eType : enemiesToCheck ) {
			EntityList& enemyList = m_entities[ eType ];

			// For every enemy of that type
			for( unsigned int enemyIndex = 0; enemyIndex < enemyList.size(); enemyIndex++ )
			{
				Entity* thisEnemy = enemyList[ enemyIndex ];

				// Check for collision
				if( Disc2::DoDiscsOverlap( thisTank->m_collosionDisc2 , thisEnemy->m_collosionDisc2 ) )
				{
					float totalPushBack = thisTank->m_collosionDisc2.radius + thisEnemy->m_collosionDisc2.radius - Vector2::GetDistance( thisTank->GetPosition() , thisEnemy->GetPosition() );

					float enemyPushBack = totalPushBack / 2.f;
					float playerPushBack = totalPushBack / 2.f;
					if( eType == ENTITY_TYPE_ENEMY_TURRET ) 
					{
						// EnemyTurrets don't get pushed back
						enemyPushBack = 0.f;
						playerPushBack = totalPushBack;
					}

					Vector2 playerPushBackDirection = thisTank->GetPosition() - thisEnemy->GetPosition();
					playerPushBackDirection.NormalizeAndGetLength();
					thisTank->SetPosition( thisTank->GetPosition() + (playerPushBackDirection * playerPushBack) );

					Vector2 enemyPushBackDirection = thisEnemy->GetPosition() - thisTank->GetPosition();
					enemyPushBackDirection.NormalizeAndGetLength();
					thisEnemy->SetPosition( thisEnemy->GetPosition() + (enemyPushBackDirection * enemyPushBack) );

				}
			}
		}
	}
}

void Map::CheckEnemiesVsEnemiesCollision()
{
	EntityType enemiesToCheck[] = {
		ENTITY_TYPE_ENEMY_TURRET,
		ENTITY_TYPE_ENEMY_TANK
	};

	// For every enemy types
	for( EntityType& eType : enemiesToCheck ) {
		EntityList& enemyList = m_entities[ eType ];

		// For every enemy of that type
		for( unsigned int enemyIndex = 0; enemyIndex < enemyList.size(); enemyIndex++ )
		{
			// For every other enemy types
			for( EntityType& eOtherType : enemiesToCheck ) {
				EntityList& otherEnemyList = m_entities[ eOtherType ];

				// For every other enemy of that type
				for( unsigned int otherEnemyIndex = 0; otherEnemyIndex < otherEnemyList.size(); otherEnemyIndex++ )
				{
					Entity* thisEnemy = enemyList[ enemyIndex ];
					Entity* otherEnemy = otherEnemyList[ otherEnemyIndex ];
					
					// If both are different enemies
					if( thisEnemy != otherEnemy )
					{
						// Check for collision
						if( Disc2::DoDiscsOverlap( thisEnemy->m_collosionDisc2 , otherEnemy->m_collosionDisc2 ) )
						{
							float totalPushBack = thisEnemy->m_collosionDisc2.radius + otherEnemy->m_collosionDisc2.radius - Vector2::GetDistance( thisEnemy->GetPosition() , otherEnemy->GetPosition() );

							float otherEnemyPushBack = totalPushBack / 2.f;
							float thisEnemyPushBack = totalPushBack / 2.f;
							if( eOtherType == ENTITY_TYPE_ENEMY_TURRET ) 
							{
								// EnemyTurrets don't get pushed back
								otherEnemyPushBack = 0.f;
								thisEnemyPushBack = totalPushBack;
							}

							if( eType == ENTITY_TYPE_ENEMY_TURRET ) 
							{
								// EnemyTurrets don't get pushed back
								thisEnemyPushBack = 0.f;
								otherEnemyPushBack = totalPushBack;
							}

							Vector2 thisEnemyPushBackDirection = thisEnemy->GetPosition() - otherEnemy->GetPosition();
							thisEnemyPushBackDirection.NormalizeAndGetLength();
							thisEnemy->SetPosition( thisEnemy->GetPosition() + (thisEnemyPushBackDirection * thisEnemyPushBack) );

							Vector2 otherEnemyPushBackDirection = otherEnemy->GetPosition() - thisEnemy->GetPosition();
							otherEnemyPushBackDirection.NormalizeAndGetLength();
							otherEnemy->SetPosition( otherEnemy->GetPosition() + (otherEnemyPushBackDirection * otherEnemyPushBack) );

						}
					}
				}
			}
		}
	}
}