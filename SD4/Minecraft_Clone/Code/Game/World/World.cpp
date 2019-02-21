#pragma once
#include "World.hpp"
#include <algorithm>
#include "Game/GameCommon.hpp"

World::World( Clock *parentClock )
	: m_clock( parentClock )
{
	// Setting up the Camera
	m_camera = new MCamera( *g_theRenderer );
	m_camera->m_cameraNear = 0.01f;
	m_camera->m_cameraFar = 2000.f;

	m_camera->m_position = Vector3( -3.f, 3.f, BLOCKS_WIDE_Z );
	m_camera->m_yawDegreesAboutZ = -40.f;
	m_camera->SetPitchDegreesAboutY( 25.f );

	// Activation Cheat sheet
	PopulateChunkActivationCheatsheet( m_deactivationRadius );
}

World::~World()
{
	for( ChunkMap::iterator it = m_activeChunks.begin(); it != m_activeChunks.end(); it = m_activeChunks.begin() )
	{
		delete it->second;
		it->second = nullptr;

		m_activeChunks.erase( it );
	}

	// Camera
	delete m_camera;
	m_camera = nullptr;
}

void World::Update()
{
	float const deltaSeconds = (float) m_clock.GetFrameDeltaSeconds();

	// Moves the camera, for now
	ProcessInput( deltaSeconds );

	// Chunk Management
	ActivateChunkNearestToPosition( m_camera->m_position );
	DeactivateChunkForPosition( m_camera->m_position );
	RebuiltOneChunkIfRequired( m_camera->m_position );

	// Block Selection
	PerformRaycast();

	m_camera->RebuildMatrices();
}

void World::Render() const
{
	// Ambient Light
	g_theRenderer->SetAmbientLight( m_ambientLight );

	// Camera
	Camera &camera = *m_camera->GetCamera();
	g_theRenderer->BindCamera( &camera );
	g_theRenderer->ClearColor( RGBA_BLACK_COLOR );
	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true );

	// Pre Render
	camera.PreRender( *g_theRenderer );

	//----------------------------
	// The Rendering starts here..
	//	
	for( ChunkMap::const_iterator it = m_activeChunks.begin(); it != m_activeChunks.end(); it++ )
	{
		Chunk const *thisChunk = it->second;

		if( thisChunk->HasMesh() )
			thisChunk->Render( *g_theRenderer );
	}

	// Raycast Selection
	RenderBlockSelection( m_blockSelectionRaycastResult );
	if( m_raycastIsLocked )
		RenderRaycast( m_blockSelectionRaycastResult, *g_theRenderer );

	// Post Render
	camera.PostRender( *g_theRenderer );
}

RaycastResult_MC World::Raycast( Vector3 const &start, Vector3 const &forwardDir, float maxDistance ) const
{
	// Settings
	float const stepSize = 0.01f;
	int const totalSteps = (int)(maxDistance/stepSize);

	// Pre-calculated results for edge cases
	BlockLocator const startBlockLocator	= GetBlockLocatorForWorldPosition( start );
	BlockLocator const endBlockLocator		= GetBlockLocatorForWorldPosition( start + (forwardDir * maxDistance) );
	RaycastResult_MC const &startInSolidResult	= RaycastResult_MC( start, forwardDir, maxDistance, 0.f, startBlockLocator, -forwardDir );
	RaycastResult_MC const &noImpactResult		= RaycastResult_MC( start, forwardDir, maxDistance, 1.f, endBlockLocator, Vector3::ZERO );

	// Started in solid
	if( startBlockLocator.GetBlock().GetType() != BLOCK_AIR )
		return startInSolidResult;

	// Starting from an AIR block
	BlockLocator previousBlockLocator = startBlockLocator;

	// Step-and-sample
	for( int stepNum = 1; stepNum <= totalSteps; stepNum++ )
	{
		float const distanceTravelled = stepSize * stepNum;

		Vector3 currentPosition = start + (forwardDir * distanceTravelled);
		BlockLocator currentBlockLocator = GetBlockLocatorForWorldPosition( currentPosition );

		// If on the same block as before, proceed to the next step
		if( currentBlockLocator == previousBlockLocator )
			continue;

		// If current block is AIR
		if( currentBlockLocator.GetBlock().GetType() == BLOCK_AIR )
		{
			// No impact, keep going..
			previousBlockLocator = currentBlockLocator;
			continue;
		}

		// If current block is NOT AIR, i.e. we did impact!
		Vector3 impactNormal = ( previousBlockLocator.GetBlockWorldPosition() - currentBlockLocator.GetBlockWorldPosition() ).GetNormalized();
		RaycastResult_MC impactResult = RaycastResult_MC( start, forwardDir, maxDistance, distanceTravelled/maxDistance, currentBlockLocator, impactNormal );
		
		return impactResult;
	}

	// No impact
	return noImpactResult;
}

BlockLocator const World::GetBlockLocatorForWorldPosition( Vector3 const &worldPosition ) const
{
	ChunkCoord	chunkAtPosition			= World::ChunkCoordFromWorldPosition( worldPosition );
	Vector3		chunkWorldPosition		= Vector3( (float)chunkAtPosition.x * (float)BLOCKS_WIDE_X, (float)chunkAtPosition.y * (float)BLOCKS_WIDE_Y, 0.f );
	Vector3		blockPosChunkRelative	= Vector3( worldPosition.x - chunkWorldPosition.x, worldPosition.y - chunkWorldPosition.y, worldPosition.z - chunkWorldPosition.z );
	
	BlockCoord blockCoord;
	blockCoord.x = (int) floorf( blockPosChunkRelative.x );
	blockCoord.y = (int) floorf( blockPosChunkRelative.y );
	blockCoord.z = (int) floorf( blockPosChunkRelative.z );

	ChunkMap::const_iterator itChunk = m_activeChunks.find( chunkAtPosition );
	if( itChunk == m_activeChunks.end() )
		return BlockLocator::INVALID;
	else
		return BlockLocator( itChunk->second, Chunk::GetIndexFromBlockCoord(blockCoord) );
}

void World::RenderBasis( Vector3 const &position, float length, Renderer &activeRenderer )
{
	Vertex_3DPCU vBuffer[6];

	vBuffer[0].m_color		= RGBA_RED_COLOR;
	vBuffer[0].m_position	= position;
	vBuffer[1].m_color		= RGBA_RED_COLOR;
	vBuffer[1].m_position	= position + Vector3( length, 0.f, 0.f );


	vBuffer[2].m_color		= RGBA_GREEN_COLOR;
	vBuffer[2].m_position	= position;
	vBuffer[3].m_color		= RGBA_GREEN_COLOR;
	vBuffer[3].m_position	= position + Vector3( 0.f, length, 0.f );

	vBuffer[4].m_color		= RGBA_BLUE_COLOR;
	vBuffer[4].m_position	= position;
	vBuffer[5].m_color		= RGBA_BLUE_COLOR;
	vBuffer[5].m_position	= position + Vector3( 0.f, 0.f, length );

	// First Render Pass [ Normal ]
	activeRenderer.BindMaterialForShaderIndex( *g_defaultMaterial );
	activeRenderer.EnableDepth( COMPARE_LESS, true );
	activeRenderer.DrawMeshImmediate<Vertex_3DPCU>( vBuffer, 6, PRIMITIVE_LINES );

	// For, Second Render Pass [ X-Ray ]
	for( int i = 0; i < 6; i++ )
	{
		Rgba &vertColor = vBuffer[i].m_color;

		vertColor.r = (uchar)( (float)vertColor.r * 0.5f );
		vertColor.g = (uchar)( (float)vertColor.g * 0.5f );
		vertColor.b = (uchar)( (float)vertColor.b * 0.5f );
	}

	activeRenderer.EnableDepth( COMPARE_GREATER, false );	// To Draw X-Ray without affecting the depth
	activeRenderer.DrawMeshImmediate<Vertex_3DPCU>( vBuffer, 6, PRIMITIVE_LINES );
}

void World::RenderRaycast( RaycastResult_MC const &raycastResult, Renderer &activeRenderer )
{
	// Render line which did not hit
	RenderLineXRay( raycastResult.m_startPosition, RGBA_GREEN_COLOR, raycastResult.m_impactPosition, RGBA_GREEN_COLOR, activeRenderer );

	RenderBasis( raycastResult.m_impactPosition, 0.5f, activeRenderer );

	// Render line which did hit
	RenderLineXRay( raycastResult.m_impactPosition, RGBA_RED_COLOR, raycastResult.m_endPosition, RGBA_RED_COLOR, activeRenderer );
}

void World::RenderLineXRay( Vector3 const &startPos, Rgba const &startColor, Vector3 const &endPos, Rgba const &endColor, Renderer &activeRenderer )
{
	Vertex_3DPCU vBuffer[2];
	vBuffer[0].m_color		= startColor;
	vBuffer[0].m_position	= startPos;
	vBuffer[1].m_color		= endColor;
	vBuffer[1].m_position	= endPos;

	// First Render Pass [ Normal ]
	activeRenderer.BindMaterialForShaderIndex( *g_defaultMaterial );
	activeRenderer.EnableDepth( COMPARE_LESS, true );
	activeRenderer.DrawMeshImmediate<Vertex_3DPCU>( vBuffer, 6, PRIMITIVE_LINES );

	// For, Second Render Pass [ X-Ray ]
	for( int i = 0; i < 6; i++ )
	{
		Rgba &vertColor = vBuffer[i].m_color;

		vertColor.r = (uchar)( (float)vertColor.r * 0.5f );
		vertColor.g = (uchar)( (float)vertColor.g * 0.5f );
		vertColor.b = (uchar)( (float)vertColor.b * 0.5f );
	}

	activeRenderer.EnableDepth( COMPARE_GREATER, false );	// To Draw X-Ray without affecting the depth
	activeRenderer.DrawMeshImmediate<Vertex_3DPCU>( vBuffer, 6, PRIMITIVE_LINES );
}

void World::ProcessInput( float deltaSeconds )
{
	// Camera Rotation
	Vector2 mouseChange = g_theInput->GetMouseDelta();
	float const curentCamPitch = m_camera->GetPitchDegreesAboutY();

	m_camera->m_yawDegreesAboutZ	-= mouseChange.x * m_camRotationSpeed;
	m_camera->SetPitchDegreesAboutY( curentCamPitch + (mouseChange.y * m_camRotationSpeed) );

	// Camera Position
	float	const camYaw	 = m_camera->m_yawDegreesAboutZ;
	Vector3 const forwardDir = Vector3( CosDegree(camYaw), SinDegree(camYaw), 0.f );
	Vector3 const leftDir	 = Vector3( forwardDir.y * -1.f, forwardDir.x, 0.f );
	Vector3 const upDir		 = Vector3( 0.f, 0.f, 1.f );

	float forwardMovement = 0.f;
	float leftMovement = 0.f;
	float upMovement = 0.f;
	float speedScale = 1.f;

	if( g_theInput->IsKeyPressed( VK_SHIFT ) )
		speedScale = 6.f;
	if( g_theInput->IsKeyPressed( 'W' ) )
		forwardMovement += m_flySpeed * speedScale * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'S' ) )
		forwardMovement -= m_flySpeed * speedScale * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'A' ) )
		leftMovement += m_flySpeed * speedScale * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'D' ) )
		leftMovement -= m_flySpeed * speedScale * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'Q' ) )
		upMovement += m_flySpeed * speedScale * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'E' ) )
		upMovement -= m_flySpeed * speedScale * deltaSeconds;

	Vector3 positionChange = (forwardDir * forwardMovement) + (leftDir * leftMovement) + (upDir * upMovement);
	m_camera->m_position += positionChange;
}

void World::RebuiltOneChunkIfRequired( Vector3 const &playerWorldPos )
{
	ChunkCoord	const originChunkCoord			= ChunkCoordFromWorldPosition( playerWorldPos );
	float		const meshUpdateRadiusSuqared	= (float)(m_activationRadius * m_activationRadius);

	for( int i = 0; i < m_activationPriorityCheatSheet.size(); i++ )
	{
		ChunkCoord const &relativeOffset		= m_activationPriorityCheatSheet[i];
		ChunkCoord const  rebuilChunkAtCoord	= originChunkCoord + relativeOffset;

		// Make sure we're inside activation radius
		float distanceFromOriginSquared = (rebuilChunkAtCoord - originChunkCoord).GetLengthSquared();
		if( distanceFromOriginSquared > meshUpdateRadiusSuqared )
			return;

		// See the chunk at this coord exist in the map
		ChunkMap::iterator it = m_activeChunks.find( rebuilChunkAtCoord );
		if( it != m_activeChunks.end() )
		{
			Chunk &chunkToAct = *it->second;
			if( chunkToAct.HasAllNeighbors() && chunkToAct.IsDirty() )
			{
				// Construct the mesh
				chunkToAct.RebuildMesh();
				return;
			}
		}
	}
}

void World::ActivateChunkNearestToPosition( Vector3 const &playerWorldPos )
{
	ChunkCoord	const originChunkCoord			= ChunkCoordFromWorldPosition( playerWorldPos );
	float		const activationRadiusSquared	= (float)(m_activationRadius * m_activationRadius);

	// Activate the next chunk according to priority..
	for( int i = 0; i < m_activationPriorityCheatSheet.size(); i++ )
	{
		ChunkCoord const &relativeOffset		= m_activationPriorityCheatSheet[i];
		ChunkCoord const  activationChunkCoord	= originChunkCoord + relativeOffset;

		// Make sure we're inside activation radius
		float distanceFromOriginSquared = (activationChunkCoord - originChunkCoord).GetLengthSquared();
		if( distanceFromOriginSquared > activationRadiusSquared )
			return;

		// See the chunk at this coord exist in the map
		ChunkMap::iterator itChunkToActivate = m_activeChunks.find( activationChunkCoord );
		if( itChunkToActivate == m_activeChunks.end() )
		{
			// Create the chunk, add it to activation list
			Chunk *newChunk = new Chunk( activationChunkCoord );		// It should mark it dirty!
			m_activeChunks[ activationChunkCoord ] = newChunk;

			// Link Neighbors with each other
			ChunkMap neighbourChunks;
			GetNeighborsOfChunkAt( activationChunkCoord, neighbourChunks );
			for( ChunkMap::iterator itNeighbor = neighbourChunks.begin(); itNeighbor != neighbourChunks.end(); itNeighbor++ )
			{
				Chunk* &neighborChunk			= itNeighbor->second;
				ChunkCoord neighborChunkCoord	= itNeighbor->first;

				neighborChunk->SetNeighborAtCoordinate( newChunk, activationChunkCoord );
				newChunk->SetNeighborAtCoordinate( neighborChunk, neighborChunkCoord );
			}

			itChunkToActivate = m_activeChunks.find( activationChunkCoord );
		}
	}
}

void World::DeactivateChunkForPosition( Vector3 const &playerWorldPos )
{
	ChunkCoord	const	originChunkCoord			= ChunkCoordFromWorldPosition( playerWorldPos );
	float		const	deactivationRadiusSquared	= (float)(m_deactivationRadius * m_deactivationRadius);
	float				chunkAtMaxDistanceSquared	= 0.f;
	ChunkMap::iterator	chunkToDeactivate			= m_activeChunks.end();

	for( ChunkMap::iterator it = m_activeChunks.begin(); it != m_activeChunks.end(); it++ )
	{
		ChunkCoord	const thisChunkCoord = it->first;
		float		const distanceFromOriginSquared = (thisChunkCoord - originChunkCoord).GetLengthSquared();

		// Only do if outside deactivation radius
		if( distanceFromOriginSquared < deactivationRadiusSquared )
			continue;

		// Not the chunk at the largest distance
		if( distanceFromOriginSquared > chunkAtMaxDistanceSquared )
		{
			chunkAtMaxDistanceSquared = distanceFromOriginSquared;
			chunkToDeactivate = it;
		}
	}

	// Deactivate if we found any
	if( chunkToDeactivate != m_activeChunks.end() )
	{
		Chunk* &chunkToDelete			= chunkToDeactivate->second;
		ChunkCoord chunkCoordToDelete	= chunkToDeactivate->first;
		
		// Unlink Neighbors with each other
		ChunkMap neighbourChunks;
		GetNeighborsOfChunkAt( chunkCoordToDelete, neighbourChunks );
		for( ChunkMap::iterator itNeighbor = neighbourChunks.begin(); itNeighbor != neighbourChunks.end(); itNeighbor++ )
		{
			Chunk* &neighborChunk = itNeighbor->second;
			neighborChunk->SetNeighborAtCoordinate( nullptr, chunkCoordToDelete );
		}

		delete chunkToDelete;
		chunkToDelete = nullptr;

		m_activeChunks.erase( chunkToDeactivate );
	}
}

void World::PopulateChunkActivationCheatsheet( int deactivationRadius )
{
	m_activationPriorityCheatSheet.clear();

	for( int x = -deactivationRadius; x <= deactivationRadius; x++ )
	{
		for( int y = -deactivationRadius; y <= deactivationRadius; y++ )
			m_activationPriorityCheatSheet.push_back( ChunkCoord(x,y) );
	}

	std::sort( m_activationPriorityCheatSheet.begin(), m_activationPriorityCheatSheet.end(), World::CheetsheetCompare );
}

void World::GetNeighborsOfChunkAt( ChunkCoord const &chunkCoord, ChunkMap &neighborChunks_out )
{
	neighborChunks_out.clear();
	ChunkMap::iterator nIt;

	// East Neighbor
	ChunkCoord eastNeighborCoord  = chunkCoord + EAST_CHUNKCOORD;
	nIt = m_activeChunks.find( eastNeighborCoord );
	if( nIt != m_activeChunks.end() )
		neighborChunks_out[ nIt->first ] = nIt->second;

	// West..
	ChunkCoord westNeighborCoord  = chunkCoord + WEST_CHUNKCOORD;
	nIt = m_activeChunks.find( westNeighborCoord );
	if( nIt != m_activeChunks.end() )
		neighborChunks_out[ nIt->first ] = nIt->second;

	// North..
	ChunkCoord northNeighborCoord = chunkCoord + NORTH_CHUNKCOORD;
	nIt = m_activeChunks.find( northNeighborCoord );
	if( nIt != m_activeChunks.end() )
		neighborChunks_out[ nIt->first ] = nIt->second;

	// South..
	ChunkCoord southNeighborCoord = chunkCoord + SOUTH_CHUNKCOORD;
	nIt = m_activeChunks.find( southNeighborCoord );
	if( nIt != m_activeChunks.end() )
		neighborChunks_out[ nIt->first ] = nIt->second;
}

void World::PerformRaycast()
{
	bool raycastLockJustGotChanged = false;
	bool oldRaycastLockState = m_raycastIsLocked;

	if( g_theInput->WasKeyJustPressed('R') )
		m_raycastIsLocked = !m_raycastIsLocked;

	raycastLockJustGotChanged = (m_raycastIsLocked != oldRaycastLockState);

	// If just got locked..
	if( (raycastLockJustGotChanged == true) && m_raycastIsLocked )
	{
		m_lockedRayStartPos		= m_camera->m_position;
		m_lockedRayDirection	= m_camera->GetForwardDirection();
	}

	// If not locked..
	if( m_raycastIsLocked == false )
	{
		m_lockedRayStartPos		= m_camera->m_position;
		m_lockedRayDirection	= m_camera->GetForwardDirection();
	}

	m_blockSelectionRaycastResult = Raycast( m_lockedRayStartPos, m_lockedRayDirection, m_raycastMaxDistance );
}

void World::RenderBlockSelection( RaycastResult_MC const &raycastResult ) const
{
	if( raycastResult.DidImpact() == false )
		return;

	Vector3 const blockWorldCenter		= raycastResult.m_impactBlock.GetBlockWorldPosition() + Vector3(0.5f, 0.5f, 0.5f);
	Vector3 const blockHalfDimensions	= Vector3( 1.1f, 1.1f, 1.1f ) * 0.5f;

	/*
	      7_________ 6			VERTEX[8] ORDER:
		  /|       /|				( 0, 1, 2, 3, 4, 5, 6, 7 )
		 / | top  / |				
	   4/__|_____/5 |			
		|  |_____|__|			   z|   
		| 3/     |  /2				|  / x
		| /  bot | /				| /
		|/_______|/			y ______|/ 
		0         1
	*/
	Rgba const &blockSelectionColor = RGBA_RED_COLOR;
	Rgba const &faceSelectionColor = RGBA_BLUE_COLOR;
	Vector3 const vertexPos[8] = {
		Vector3( blockWorldCenter.x - blockHalfDimensions.x,	blockWorldCenter.y + blockHalfDimensions.y,	blockWorldCenter.z - blockHalfDimensions.z ),
		Vector3( blockWorldCenter.x - blockHalfDimensions.x,	blockWorldCenter.y - blockHalfDimensions.y,	blockWorldCenter.z - blockHalfDimensions.z ),
		Vector3( blockWorldCenter.x + blockHalfDimensions.x,	blockWorldCenter.y - blockHalfDimensions.y,	blockWorldCenter.z - blockHalfDimensions.z ),
		Vector3( blockWorldCenter.x + blockHalfDimensions.x,	blockWorldCenter.y + blockHalfDimensions.y,	blockWorldCenter.z - blockHalfDimensions.z ),
		Vector3( blockWorldCenter.x - blockHalfDimensions.x,	blockWorldCenter.y + blockHalfDimensions.y,	blockWorldCenter.z + blockHalfDimensions.z ),
		Vector3( blockWorldCenter.x - blockHalfDimensions.x,	blockWorldCenter.y - blockHalfDimensions.y,	blockWorldCenter.z + blockHalfDimensions.z ),
		Vector3( blockWorldCenter.x + blockHalfDimensions.x,	blockWorldCenter.y - blockHalfDimensions.y,	blockWorldCenter.z + blockHalfDimensions.z ),
		Vector3( blockWorldCenter.x + blockHalfDimensions.x,	blockWorldCenter.y + blockHalfDimensions.y,	blockWorldCenter.z + blockHalfDimensions.z )
	};

	Vertex_3DPCU vBuffer[32];
	// Bottom Side
	// Line 01
	vBuffer[0].m_color = blockSelectionColor;
	vBuffer[0].m_position = vertexPos[0];
	vBuffer[1].m_color = blockSelectionColor;
	vBuffer[1].m_position = vertexPos[1];

	// Line 12
	vBuffer[2].m_color = blockSelectionColor;
	vBuffer[2].m_position = vertexPos[1];
	vBuffer[3].m_color = blockSelectionColor;
	vBuffer[3].m_position = vertexPos[2];

	// Line 23
	vBuffer[4].m_color = blockSelectionColor;
	vBuffer[4].m_position = vertexPos[2];
	vBuffer[5].m_color = blockSelectionColor;
	vBuffer[5].m_position = vertexPos[3];

	// Line 30
	vBuffer[6].m_color = blockSelectionColor;
	vBuffer[6].m_position = vertexPos[3];
	vBuffer[7].m_color = blockSelectionColor;
	vBuffer[7].m_position = vertexPos[0];

	// Top Side
	// Line 45
	vBuffer[8].m_color = blockSelectionColor;
	vBuffer[8].m_position = vertexPos[4];
	vBuffer[9].m_color = blockSelectionColor;
	vBuffer[9].m_position = vertexPos[5];

	// Line 56
	vBuffer[10].m_color = blockSelectionColor;
	vBuffer[10].m_position = vertexPos[5];
	vBuffer[11].m_color = blockSelectionColor;
	vBuffer[11].m_position = vertexPos[6];

	// Line 67
	vBuffer[12].m_color = blockSelectionColor;
	vBuffer[12].m_position = vertexPos[6];
	vBuffer[13].m_color = blockSelectionColor;
	vBuffer[13].m_position = vertexPos[7];

	// Line 74
	vBuffer[14].m_color = blockSelectionColor;
	vBuffer[14].m_position = vertexPos[7];
	vBuffer[15].m_color = blockSelectionColor;
	vBuffer[15].m_position = vertexPos[4];
	
	// Vertical Four Edges
	// Line 40
	vBuffer[16].m_color = blockSelectionColor;
	vBuffer[16].m_position = vertexPos[4];
	vBuffer[17].m_color = blockSelectionColor;
	vBuffer[17].m_position = vertexPos[0];

	// Line 51
	vBuffer[18].m_color = blockSelectionColor;
	vBuffer[18].m_position = vertexPos[5];
	vBuffer[19].m_color = blockSelectionColor;
	vBuffer[19].m_position = vertexPos[1];

	// Line 62
	vBuffer[20].m_color = blockSelectionColor;
	vBuffer[20].m_position = vertexPos[6];
	vBuffer[21].m_color = blockSelectionColor;
	vBuffer[21].m_position = vertexPos[2];

	// Line 73
	vBuffer[22].m_color = blockSelectionColor;
	vBuffer[22].m_position = vertexPos[7];
	vBuffer[23].m_color = blockSelectionColor;
	vBuffer[23].m_position = vertexPos[3];

	// Selected Side
	float	halfFaceDim	 = 0.5f;
	Vector3 topDirection = Vector3( 0.f, 0.f, 1.f );
	if( raycastResult.m_impactNormal == topDirection )
		topDirection = Vector3( 1.f, 0.f, 0.f );

	Vector3 rightDirection	= Vector3::CrossProduct( topDirection, raycastResult.m_impactNormal );
	Vector3 sideCenter	= blockWorldCenter + (raycastResult.m_impactNormal * 0.51f);
	Vector3 ssTopLeft	= sideCenter + ( topDirection * halfFaceDim ) + (-rightDirection * halfFaceDim );
	Vector3 ssTopRight	= sideCenter + ( topDirection * halfFaceDim ) + ( rightDirection * halfFaceDim );
	Vector3 ssBotLeft	= sideCenter + (-topDirection * halfFaceDim ) + (-rightDirection * halfFaceDim );
	Vector3 ssBotRight	= sideCenter + (-topDirection * halfFaceDim ) + ( rightDirection * halfFaceDim );

	// Line top left to right
	vBuffer[24].m_color = faceSelectionColor;
	vBuffer[24].m_position = ssTopLeft;
	vBuffer[25].m_color = faceSelectionColor;
	vBuffer[25].m_position = ssTopRight;

	// Line bot left to right
	vBuffer[26].m_color = faceSelectionColor;
	vBuffer[26].m_position = ssBotLeft;
	vBuffer[27].m_color = faceSelectionColor;
	vBuffer[27].m_position = ssBotRight;

	// Line left bot to top
	vBuffer[28].m_color = faceSelectionColor;
	vBuffer[28].m_position = ssBotLeft;
	vBuffer[29].m_color = faceSelectionColor;
	vBuffer[29].m_position = ssTopLeft;

	// Line right bot to top
	vBuffer[30].m_color = faceSelectionColor;
	vBuffer[30].m_position = ssBotRight;
	vBuffer[31].m_color = faceSelectionColor;
	vBuffer[31].m_position = ssTopRight;

	// Render
	g_theRenderer->BindMaterialForShaderIndex( *g_defaultMaterial );
	g_theRenderer->EnableDepth( COMPARE_LESS, true );
	g_theRenderer->DrawMeshImmediate<Vertex_3DPCU>( vBuffer, 32, PRIMITIVE_LINES );
}

bool World::CheetsheetCompare( ChunkCoord const &a, ChunkCoord const &b )
{
	float aFromOriginDistSquared = a.GetLengthSquared();
	float bFromOriginDistSquared = b.GetLengthSquared();

	return (aFromOriginDistSquared < bFromOriginDistSquared);
}

ChunkCoord World::ChunkCoordFromWorldPosition( Vector3 const &position )
{
	Vector2 positionXY = Vector2( position.x, position.y );
	positionXY.x = floorf( positionXY.x / (float)BLOCKS_WIDE_X );
	positionXY.y = floorf( positionXY.y / (float)BLOCKS_WIDE_Y );

	return ChunkCoord( (int)positionXY.x, (int)positionXY.y );
}
