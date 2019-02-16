#pragma once
#include "World.hpp"
#include <algorithm>
#include "Game/GameCommon.hpp"

World::World( Clock *parentClock )
	: m_clock( parentClock )
{
	// Setting up the Camera
	m_camera = new MCamera( *g_theRenderer );
	m_camera->m_cameraNear = 0.001f;
	m_camera->m_cameraFar = 2000.f;

	m_camera->m_position = Vector3( -3.f, 3.f, BLOCKS_WIDE_Z );
	m_camera->m_yawDegreesAboutZ = -40.f;
	m_camera->SetPitchDegreesAboutY( 25.f );

	// Activation Cheatsheet
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

	// Post Render
	camera.PostRender( *g_theRenderer );
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
			if( chunkToAct.IsDirty() )
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
		ChunkMap::iterator it = m_activeChunks.find( activationChunkCoord );
		if( it == m_activeChunks.end() )
		{
			// Create the chunk, add it to activation list
			Chunk *newChunk = new Chunk( activationChunkCoord );		// It should mark it dirty!
			m_activeChunks[ activationChunkCoord ] = newChunk;

			it = m_activeChunks.find( activationChunkCoord );
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
		Chunk* &chunkToDelete = chunkToDeactivate->second;

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
