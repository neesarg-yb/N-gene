#pragma once
#include "World.hpp"
#include "Game/GameCommon.hpp"

World::World( Clock *parentClock )
	: m_clock( parentClock )
{
	// Setting up the Camera
	m_camera = new MCamera( *g_theRenderer );
	m_camera->m_position = Vector3( -3.f, 3.f, 3.f );
	m_camera->m_yawDegreesAboutZ = -40.f;
	m_camera->SetPitchDegreesAboutY( 25.f );

	ChunkCoord chunk1Coord( 0, 0 );
	Chunk *chunk1 = new Chunk( chunk1Coord );
	ChunkCoord chunk2Coord( -1, 2 );
	Chunk *chunk2 = new Chunk( chunk2Coord );
	ChunkCoord chunk3Coord( 0, 2 );
	Chunk *chunk3 = new Chunk( chunk3Coord );
	ChunkCoord chunk4Coord( 1, 2 );
	Chunk *chunk4 = new Chunk( chunk4Coord );
	ChunkCoord chunk5Coord( 0, 3 );
	Chunk *chunk5 = new Chunk( chunk5Coord );

	m_activeChunks.insert( std::pair<ChunkCoord, Chunk*>( chunk1Coord, chunk1 ) );
	m_activeChunks.insert( std::pair<ChunkCoord, Chunk*>( chunk2Coord, chunk2 ) );
	m_activeChunks.insert( std::pair<ChunkCoord, Chunk*>( chunk3Coord, chunk3 ) );
	m_activeChunks.insert( std::pair<ChunkCoord, Chunk*>( chunk4Coord, chunk4 ) );
	m_activeChunks.insert( std::pair<ChunkCoord, Chunk*>( chunk5Coord, chunk5 ) );
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
	ProcessInput( deltaSeconds );

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

	if( g_theInput->IsKeyPressed( 'W' ) )
		forwardMovement += m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'S' ) )
		forwardMovement -= m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'A' ) )
		leftMovement += m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'D' ) )
		leftMovement -= m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'Q' ) )
		upMovement += m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'E' ) )
		upMovement -= m_flySpeed * deltaSeconds;

	Vector3 positionChange = (forwardDir * forwardMovement) + (leftDir * leftMovement) + (upDir * upMovement);
	m_camera->m_position += positionChange;
}

Block* World::GetChunkAtChunkCoordinates( IntVector2 const &chunckCoord )
{
	UNUSED( chunckCoord );

	return nullptr;
}
