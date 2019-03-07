#pragma once
#include "World.hpp"
#include <algorithm>
#include "Engine/DebugRenderer/DebugRenderer.hpp"
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

	// Lighting
	UpdateDirtyLighting();

	RebuiltOneChunkIfRequired( m_camera->m_position );

	DeactivateChunkForPosition( m_camera->m_position );

	// Block Selection
	PerformRaycast();
	PlaceOrDigBlock();

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

	// Dirty Lights Debug
	if( DEBUG_RENDER_DIRTY_LIGHTS )
		RenderDirtyLightMesh();

	if( m_raycastIsLocked )
		RenderRaycast( m_blockSelectionRaycastResult, *g_theRenderer );

	// Post Render
	camera.PostRender( *g_theRenderer );

	DebugRenderInputKeyInfo();
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

void World::DebugRenderInputKeyInfo() const
{
	Vector2 mouseClickTxtPos = Vector2( -820.f, 450.f );
	DebugRender2DText( 0.f, mouseClickTxtPos, 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "Mouse Clicks: [Left - Dig] [Right - Place]" );
	Vector2 movementTxtPos = mouseClickTxtPos + Vector2( 0.f, -20.f );
	DebugRender2DText( 0.f, movementTxtPos, 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "Movement: [W, A, S, D - Horizontal] [Q, E - Vertical]" );
	Vector2 shiftFastTxtPos = movementTxtPos + Vector2( 0.f, -20.f );
	DebugRender2DText( 0.f, shiftFastTxtPos, 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "Faster  : [SHIFT]" );

	Vector2 placeTestBlockTxtPos = shiftFastTxtPos + Vector2( 0.f, -20.f );
	DebugRender2DText( 0.f, placeTestBlockTxtPos, 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "White Test-Block: [CTRL]" );
	Vector2 placeGlowStoneTxtPos = placeTestBlockTxtPos + Vector2( 0.f, -20.f );
	DebugRender2DText( 0.f, placeGlowStoneTxtPos, 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "Glowstone Block : [CTRL][SHIFT]" );

	Vector2 lightStepTxtPos = placeGlowStoneTxtPos + Vector2( 0.f, -20.f );
	if( DEBUG_STEP_LIGHTING )
		DebugRender2DText( 0.f, lightStepTxtPos, 15.f, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR, "Step-Light: [L]" );
	Vector2 raycastTxtPos = lightStepTxtPos + Vector2( 0.f, -20.f );
	DebugRender2DText( 0.f, raycastTxtPos, 15.f, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR, "Raycast   : [R]" );
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
			Chunk *newChunk = new Chunk( activationChunkCoord, *this );		// It should mark it dirty!
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

void World::UpdateDirtyLighting()
{
	// Dirty light debug mesh
	if( DEBUG_RENDER_DIRTY_LIGHTS )
		UpdateDirtyLightDebugMesh();

	BlockLocQue  dirtyBlocksThisFrame;
	BlockLocQue *operationQue = &m_dirtyLightBlocks;
	
	if( DEBUG_STEP_LIGHTING )
	{
		if( g_theInput->WasKeyJustPressed('L') == false )
			return;

		// If debugging, we'll operate on the dirty lights just for this frame!
		dirtyBlocksThisFrame.swap( m_dirtyLightBlocks );
		operationQue = &dirtyBlocksThisFrame;
	}

	while( operationQue->size() > 0 )
	{
		// Pop the front
		BlockLocator fDirtyBlock = operationQue->front();
		operationQue->pop_front();

		// Remove its dirty flag
		fDirtyBlock.GetBlock().ClearIsLightDirty();

		// Computes the block's theoretical indoor-outdoor lighting
		RecomputeLighting( fDirtyBlock );
	}
}

void World::UpdateDirtyLightDebugMesh()
{
	if( m_dirtyLightsMesh != nullptr )
	{
		delete m_dirtyLightsMesh;
		m_dirtyLightsMesh = nullptr;
	}

	m_dirtyLightsMesh = new MeshBuilder();
	m_dirtyLightsMesh->Begin( PRIMITIVE_POINTS, false );

	for( int i = 0; i < m_dirtyLightBlocks.size(); i++ )
	{
		BlockLocator const &dBlockLoc = m_dirtyLightBlocks[i];
		Vector3 debugPointPosition = dBlockLoc.GetBlockWorldPosition() + Vector3( 0.5f, 0.5f, 0.5f );

		m_dirtyLightsMesh->SetColor( RGBA_RED_COLOR );
		m_dirtyLightsMesh->PushVertex( debugPointPosition );
	}

	m_dirtyLightsMesh->End();
}

void World::RecomputeLighting( BlockLocator &blockLocator )
{
	Block &dBlock = blockLocator.GetBlock();

	// Sky has the highest outdoor light!
	if( dBlock.IsSky() )
		dBlock.SetOutdoorLightLevel( 14 );

	// Block's lighting never goes below these
	int const minIndoorLightLevel	= dBlock.GetIndoorLightLevelFromDefinition();
	int const minOutdoorLightLevel	= dBlock.GetOutdoorLightLevel();

	// Compute the final lighting
	int finalIndoorLighting  = 0;
	int finalOutdoorLighting = 0;

	if( dBlock.IsFullyOpaque() == false )
	{
		// Not fully opaque => it CAN receive light from neighbors
		int maxIndoorLightFromNeighbors  = 0;
		int maxOutdoorLightFromNeighbors = 0;
		GetMaxIncomingLightFromNeighbors( blockLocator, maxIndoorLightFromNeighbors, maxOutdoorLightFromNeighbors );

		// The max value wins: inherent or incoming
		finalIndoorLighting	 = max( minIndoorLightLevel,  maxIndoorLightFromNeighbors  );
		finalOutdoorLighting = max( minOutdoorLightLevel, maxOutdoorLightFromNeighbors );
	}
	else
	{
		// Fully opaque => it CAN'T receive light from neighbors
		finalIndoorLighting  = minIndoorLightLevel;
		finalOutdoorLighting = minOutdoorLightLevel;
	}
	
	// Set Indoor Light
	if( dBlock.GetIndoorLightLevel() != finalIndoorLighting )
	{
		// Only if calculated light levels are different
		blockLocator.GetChunk()->SetDirty();

		dBlock.SetIndoorLightLevel( finalIndoorLighting );
		MarkNeighborsDirtyForLighting( blockLocator );
	}

	// Set Outdoor Light
	if( dBlock.GetOutdoorLightLevel() != finalOutdoorLighting )
	{
		// Only if calculated light levels are different
		blockLocator.GetChunk()->SetDirty();

		dBlock.SetOutdoorLightLevel( finalOutdoorLighting );
		MarkNeighborsDirtyForLighting( blockLocator );
	}
}

void World::MarkLightDirtyAndAddUniqueToQueue( BlockLocator &toBeDirtyBlockLoc )
{
	if( toBeDirtyBlockLoc.IsValid() == false )
		return;

	Block &toBeDirtyBlock = toBeDirtyBlockLoc.GetBlock();
	if( toBeDirtyBlock.IsLightDirty() )
		return;								// It's already in the list

	toBeDirtyBlock.SetIsLightDirty();
	m_dirtyLightBlocks.push_back( toBeDirtyBlockLoc );

}

void World::GetMaxIncomingLightFromNeighbors( BlockLocator const &receivingBlock, int &maxIndoorLightReceived_out, int &maxOutdoorLightReceived_out ) const
{
	maxIndoorLightReceived_out  = 0;
	maxOutdoorLightReceived_out = 0;

	// Influence from North
	Block &northNeighbor	 = receivingBlock.GetNorthBlockLocator().GetBlock();
	int northMaxIndoorLight	 = northNeighbor.GetIndoorLightLevel() - 1;
	int northMaxOutdoorLight = northNeighbor.GetOutdoorLightLevel() - 1;
	maxIndoorLightReceived_out	= ( northMaxIndoorLight  > maxIndoorLightReceived_out  ) ? northMaxIndoorLight  : maxIndoorLightReceived_out;
	maxOutdoorLightReceived_out	= ( northMaxOutdoorLight > maxOutdoorLightReceived_out ) ? northMaxOutdoorLight : maxOutdoorLightReceived_out;

	// Influence from South
	Block &southNeighbor	 = receivingBlock.GetSouthBlockLocator().GetBlock();
	int southMaxIndoorLight	 = southNeighbor.GetIndoorLightLevel() - 1;
	int southMaxOutdoorLight = southNeighbor.GetOutdoorLightLevel() - 1;
	maxIndoorLightReceived_out	= ( southMaxIndoorLight  > maxIndoorLightReceived_out  ) ? southMaxIndoorLight  : maxIndoorLightReceived_out;
	maxOutdoorLightReceived_out	= ( southMaxOutdoorLight > maxOutdoorLightReceived_out ) ? southMaxOutdoorLight : maxOutdoorLightReceived_out;

	// Influence from East
	Block &eastNeighbor		 = receivingBlock.GetEastBlockLocator().GetBlock();
	int eastMaxIndoorLight	 = eastNeighbor.GetIndoorLightLevel() - 1;
	int eastMaxOutdoorLight	 = eastNeighbor.GetOutdoorLightLevel() - 1;
	maxIndoorLightReceived_out	= ( eastMaxIndoorLight  > maxIndoorLightReceived_out  ) ? eastMaxIndoorLight  : maxIndoorLightReceived_out;
	maxOutdoorLightReceived_out	= ( eastMaxOutdoorLight > maxOutdoorLightReceived_out ) ? eastMaxOutdoorLight : maxOutdoorLightReceived_out;

	// Influence from West
	Block &westNeighbor		 = receivingBlock.GetWestBlockLocator().GetBlock();
	int westMaxIndoorLight	 = westNeighbor.GetIndoorLightLevel() - 1;
	int westMaxOutdoorLight	 = westNeighbor.GetOutdoorLightLevel() - 1;
	maxIndoorLightReceived_out	= ( westMaxIndoorLight  > maxIndoorLightReceived_out  ) ? westMaxIndoorLight  : maxIndoorLightReceived_out;
	maxOutdoorLightReceived_out	= ( westMaxOutdoorLight > maxOutdoorLightReceived_out ) ? westMaxOutdoorLight : maxOutdoorLightReceived_out;

	// Influence from Down
	Block &downNeighbor		 = receivingBlock.GetDownBlockLocator().GetBlock();
	int downMaxIndoorLight	 = downNeighbor.GetIndoorLightLevel() - 1;
	int downMaxOutdoorLight	 = downNeighbor.GetOutdoorLightLevel() - 1;
	maxIndoorLightReceived_out	= ( downMaxIndoorLight  > maxIndoorLightReceived_out  ) ? downMaxIndoorLight  : maxIndoorLightReceived_out;
	maxOutdoorLightReceived_out	= ( downMaxOutdoorLight > maxOutdoorLightReceived_out ) ? downMaxOutdoorLight : maxOutdoorLightReceived_out;

	// Influence from Up
	Block &upNeighbor		 = receivingBlock.GetUpBlockLocator().GetBlock();
	int upMaxIndoorLight	 = upNeighbor.GetIndoorLightLevel() - 1;
	int upMaxOutdoorLight	 = upNeighbor.GetOutdoorLightLevel() - 1;
	maxIndoorLightReceived_out	= ( upMaxIndoorLight  > maxIndoorLightReceived_out  ) ? upMaxIndoorLight  : maxIndoorLightReceived_out;
	maxOutdoorLightReceived_out	= ( upMaxOutdoorLight > maxOutdoorLightReceived_out ) ? upMaxOutdoorLight : maxOutdoorLightReceived_out;
}

void World::MarkBlocksLightingDirtyForDig( BlockLocator &targetBlockLoc )
{
	// Mark yourself dirty
	MarkLightDirtyAndAddUniqueToQueue( targetBlockLoc );

	BlockLocator upBlockLoc	= targetBlockLoc.GetUpBlockLocator();
	Block		&upBlock	= upBlockLoc.GetBlock();
	
	if( upBlockLoc.IsValid() && (upBlock.IsSky() == false) )
		return;

	// Only if up block is sky, we'll mark all the blocks below, including ourself, as sky
	BlockLocator blockLocBelow = targetBlockLoc;
	while( blockLocBelow.IsValid() )
	{
		Block &blockBelow = blockLocBelow.GetBlock();

		if( blockBelow.IsFullyOpaque() )
			return;								// We reached the ground

		blockBelow.SetIsSky();
		MarkLightDirtyAndAddUniqueToQueue( blockLocBelow );

		// Move to next down block
		blockLocBelow = blockLocBelow.GetDownBlockLocator();
	}
}

void World::MarkBlocksLightingDirtyForPlace( BlockLocator &targetBlockLoc, eBlockType newType )
{
	Block const &targetBlock = targetBlockLoc.GetBlock();

	// Mark yourself dirty
	MarkLightDirtyAndAddUniqueToQueue( targetBlockLoc );

	// If we're about to replace a sky block with a non-air "opaque" block
	if( targetBlock.IsSky() && (newType != BLOCK_AIR) )
	{
		BlockLocator blockLocBelow = targetBlockLoc.GetDownBlockLocator();

		// Mark all the non-fully opaque blocks, below, as not sky
		while( blockLocBelow.IsValid() )
		{
			Block &blockBelow = blockLocBelow.GetBlock();

			if( blockBelow.IsFullyOpaque() )
				return;							// We reached to the ground

			blockBelow.ClearIsSky();
			MarkLightDirtyAndAddUniqueToQueue( blockLocBelow );
			
			// Move to next down block
			blockLocBelow = blockLocBelow.GetDownBlockLocator();
		}
	}
}

void World::MarkNeighborsDirtyForLighting( BlockLocator &thisBlockLoc )
{
	BlockLocator northBL = thisBlockLoc.GetNorthBlockLocator();
	BlockLocator eastBL	 = thisBlockLoc.GetEastBlockLocator();
	BlockLocator southBL = thisBlockLoc.GetSouthBlockLocator();
	BlockLocator westBL	 = thisBlockLoc.GetWestBlockLocator();
	BlockLocator upBL	 = thisBlockLoc.GetUpBlockLocator();
	BlockLocator downBL	 = thisBlockLoc.GetDownBlockLocator();

	Block &north = northBL.GetBlock();
	if( northBL.IsValid() && north.IsFullyOpaque() == false && north.IsLightDirty() == false )
	{
		northBL.GetChunk()->SetDirty();
		MarkLightDirtyAndAddUniqueToQueue( northBL );
	}

	Block &east = eastBL.GetBlock();
	if( eastBL.IsValid() && east.IsFullyOpaque() == false && east.IsLightDirty() == false )
	{
		eastBL.GetChunk()->SetDirty();
		MarkLightDirtyAndAddUniqueToQueue( eastBL );
	}

	Block &south = southBL.GetBlock();
	if( southBL.IsValid() && south.IsFullyOpaque() == false && south.IsLightDirty() == false )
	{
		southBL.GetChunk()->SetDirty();
		MarkLightDirtyAndAddUniqueToQueue( southBL );
	}

	Block &west = westBL.GetBlock();
	if( westBL.IsValid() && west.IsFullyOpaque() == false && west.IsLightDirty() == false )
	{
		westBL.GetChunk()->SetDirty();
		MarkLightDirtyAndAddUniqueToQueue( westBL );
	}

	Block &up = upBL.GetBlock();
	if( upBL.IsValid() && up.IsFullyOpaque() == false && up.IsLightDirty() == false )
	{
		upBL.GetChunk()->SetDirty();
		MarkLightDirtyAndAddUniqueToQueue( upBL );
	}

	Block &down = downBL.GetBlock();
	if( downBL.IsValid() && down.IsFullyOpaque() == false && down.IsLightDirty() == false )
	{
		downBL.GetChunk()->SetDirty();
		MarkLightDirtyAndAddUniqueToQueue( downBL );
	}
}

void World::RenderDirtyLightMesh() const
{
	if( m_dirtyLightsMesh == nullptr )
		return;

	if( m_dirtyLightsMesh->m_vertices.size() > 0 )
	{
		g_theRenderer->BindMaterialForShaderIndex( *g_defaultMaterial );
		g_theRenderer->EnableDepth( COMPARE_ALWAYS, true );

		g_theRenderer->SetGLPointSize( 5.f );
		g_theRenderer->DrawMesh( *m_dirtyLightsMesh->ConstructMesh<Vertex_3DPCU>() );
	}
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

void World::PlaceOrDigBlock()
{
	if( m_blockSelectionRaycastResult.DidImpact() == false )
		return;

	bool digABlock	 = g_theInput->WasMousButtonJustPressed( MOUSE_BUTTON_LEFT );
	bool placeABlock = g_theInput->WasMousButtonJustPressed( MOUSE_BUTTON_RIGHT );

	if( digABlock )
	{
		BlockLocator selectedBlock = m_blockSelectionRaycastResult.m_impactBlock;
		if( selectedBlock.IsValid() )
		{
			MarkBlocksLightingDirtyForDig( selectedBlock );

			selectedBlock.ChangeTypeTo( BLOCK_AIR );
			selectedBlock.SetNeighborBlockChunksDirty();
		}
	}
	else if( placeABlock )
	{
		BlockLocator selectedBlock	= m_blockSelectionRaycastResult.m_impactBlock;
		BlockLocator targetBlock	= GetBlockLocatorForWorldPosition( selectedBlock.GetBlockWorldPosition() + m_blockSelectionRaycastResult.m_impactNormal );
		if( targetBlock.IsValid() )
		{
			eBlockType newBlockType = BLOCK_STONE;

			if( g_theInput->IsKeyPressed( VK_Codes::CONTROL ) )
			{
				newBlockType = BLOCK_TEST_WHITE;

				if( g_theInput->IsKeyPressed( VK_Codes::SHIFT ) )
					newBlockType = BLOCK_GLOWSTONE;
			}

			MarkBlocksLightingDirtyForPlace( targetBlock, newBlockType );

			targetBlock.ChangeTypeTo( newBlockType );
			targetBlock.SetNeighborBlockChunksDirty();
		}
	}
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
	Rgba const &faceSelectionColor  = RGBA_WHITE_COLOR;
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
	if( fabsf( Vector3::DotProduct(raycastResult.m_impactNormal, topDirection) ) == 1.f  )
		topDirection = Vector3( 1.f, 0.f, 0.f );

	Vector3 rightDirection	= Vector3::CrossProduct( topDirection, raycastResult.m_impactNormal );
	Vector3 sideCenter	= blockWorldCenter + (raycastResult.m_impactNormal * 0.52f);
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
	g_theRenderer->EnableDepth( COMPARE_LESS, false );
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
