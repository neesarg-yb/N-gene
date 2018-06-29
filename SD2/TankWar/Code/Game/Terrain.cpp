#pragma once
#include "Terrain.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Scene.hpp"

Terrain::Terrain( Vector3 spawnPosition, IntVector2 gridSize, float maxHeight, std::string heightMapImagePath )
	: GameObject( GAME_OBJECT_TERRAIN )
	, m_maxHeight( maxHeight )
	, m_sampleSize( gridSize )
{
	// NOTE!
	//
	// TERRAIN'S ORIGIN IS AT BOTTOM LEFT CORNER, NOT AT THE CENTER!

	// Set transform
	m_transform = Transform( spawnPosition, Vector3::ZERO, Vector3::ONE_ALL );

	// Load HeightMap Image
	m_heightMapImage = new Image( heightMapImagePath );

	// Set Chunks
	m_chunks = MakeChunksUsingSurfacePatch( [this]( float u, float v ) { return this->GetVertexPositionUsingHeightMap(u,v); }, 
								 IntVector2( 50, 52 ) );

	Vector3 boundsMin	= m_transform.GetWorldPosition() + Vector3::ZERO;
	Vector3 boundsMax	= m_transform.GetWorldPosition() + Vector3( (float)m_sampleSize.x, maxHeight, (float)m_sampleSize.y );
	m_worldBounds		= AABB3( boundsMin, boundsMax );
}

Terrain::~Terrain()
{
	delete m_renderable;

	for( uint i = 0; i < m_chunks.size(); i++ )
		delete m_chunks[i];
}

void Terrain::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Terrain::AddRenderablesToScene( Scene &activeScene )
{
	// Add all chunks to scene
	for( uint i = 0; i < m_chunks.size(); i++ )
		activeScene.AddRenderable( *m_chunks[i] );
}

void Terrain::RemoveRenderablesFromScene( Scene &activeScene )
{
	// Remove all chunks to scene
	for( uint i = 0; i < m_chunks.size(); i++ )
		activeScene.RemoveRenderable( *m_chunks[i] );
}

float Terrain::GetYCoordinateForMyPositionAt( Vector2 myXZPosition, float yOffset /* = 0.f */ )
{
	// Note: myXZPosition is in World Space
	// You'll get corner's position in Terrain Space by passing corresponding local XZCoord
	// After that you need to convert it to world space before three lerps

	//  Logic:
	//  
	//  Hb = lerp( bl_h, br_h, frac_x );
	//  Ht = lerp( tl_h, tr_h, frac.x );
	//  H  = lerp( Hb, Ht, frac.y );

	// Get XZ position relative to Terrain
	Vector3 terrainWorldPos	= m_transform.GetWorldPosition();
	Vector2 posOnTerrain	= myXZPosition - Vector2( terrainWorldPos.x, terrainWorldPos.z );

	Vector2 xzFraction;
	xzFraction.x = fmodf( posOnTerrain.x, 1.f );
	xzFraction.y = fmodf( posOnTerrain.y, 1.f );

	// It will be local to terrain
	Vector3 bottomLeft	= GiveQuadVertexForMyPositionAt( posOnTerrain, TERRAIN_QUAD_BOTTOM_LEFT  );
	Vector3 bottomRight = GiveQuadVertexForMyPositionAt( posOnTerrain, TERRAIN_QUAD_BOTTOM_RIGHT );
	Vector3 topLeft		= GiveQuadVertexForMyPositionAt( posOnTerrain, TERRAIN_QUAD_TOP_LEFT	 );
	Vector3 topRight	= GiveQuadVertexForMyPositionAt( posOnTerrain, TERRAIN_QUAD_TOP_RIGHT	 );

	// Convert Y coordinate from local( terrain space ) to world
	bottomLeft.y		+= terrainWorldPos.y;
	bottomRight.y		+= terrainWorldPos.y;
	topLeft.y			+= terrainWorldPos.y;
	topRight.y			+= terrainWorldPos.y;

	float	heightBottom	= Interpolate( bottomLeft.y,	bottomRight.y,	xzFraction.x );
	float	heightTop		= Interpolate( topLeft.y,		topRight.y,		xzFraction.x );
	float	myHeight			= Interpolate( heightBottom,	heightTop,		xzFraction.y );

	return myHeight + yOffset;
}

Vector3 Terrain::Get3DCoordinateForMyPositionAt( Vector2 myXZPosition, float yOffset /* = 0.f */ )
{
	float yCoord = GetYCoordinateForMyPositionAt( myXZPosition, yOffset );

	return Vector3( myXZPosition.x, yCoord, myXZPosition.y );
}

Matrix44 Terrain::GetModelMatrixForMyPositionAt( Vector2 myXZPosition, Vector2 ForwardDirection, Vector2 RightDirection )
{
	// Get XZ position relative to Terrain
	Vector3 terrainWorldPos	= m_transform.GetWorldPosition();
	Vector2 posOnTerrain	= myXZPosition - Vector2( terrainWorldPos.x, terrainWorldPos.z );

	float	myY				= GetYCoordinateForMyPositionAt( myXZPosition );
	Vector3 myPosition		= Vector3( myXZPosition.x, myY, myXZPosition.y );

	Vector3 du				= GetVertexPositionUsingHeightMap( posOnTerrain + RightDirection * 0.5f )
							- GetVertexPositionUsingHeightMap( posOnTerrain - RightDirection * 0.5f );
	Vector3	dv				= GetVertexPositionUsingHeightMap( posOnTerrain + ForwardDirection * 0.5f )
							- GetVertexPositionUsingHeightMap( posOnTerrain - ForwardDirection * 0.5f );

	Vector3 tangent			= du.GetNormalized();
	Vector3 bitangent		= dv.GetNormalized();
	Vector3 normal			= Vector3::CrossProduct( bitangent, tangent );

	Matrix44 newModel		= Matrix44( tangent, normal, bitangent, myPosition );

	return newModel;
}

RaycastResult Terrain::Raycast( Vector3 const &startPosition, Vector3 direction, float const maxDistance )
{
	// LOGIC:
	//
	// For each Step size
		// Move Forward
			// If myY <= terrainY
				// return RaycastResult
			// Else, 
				// If max distance is reached, return noHit Result
				// Else continue..
	
	RaycastResult	hitReasult	= RaycastResult( startPosition );
					direction	= direction.GetNormalized();
	Vector3			step		= direction * 1.f;

	GUARANTEE_OR_DIE( maxDistance != 0,				"Error: maxDistance can't be ZERO!!" );
	GUARANTEE_OR_DIE( direction.GetLength() != 0.f, "Error: Infinite Raycast; the direction is ZERO!" );

	// While we reached maxDistance
	float	distanceTravelled	= 0.f;
	Vector3 currentPos			= startPosition;

	while ( distanceTravelled <= maxDistance )
	{
		// Set loop condition: distanceTravelled
		currentPos			+= step;
		distanceTravelled	 = ( currentPos - startPosition ).GetLength();

		// If we just went under the Terrain
		float terrainHeight	 = GetYCoordinateForMyPositionAt( currentPos.x, currentPos.z );
		if( currentPos.y <= terrainHeight )
		{
			hitReasult.didImpact			= true;
			hitReasult.impactPosition		= currentPos;
			hitReasult.fractionTravelled	= distanceTravelled / maxDistance;
			
			return hitReasult;
		}
		else
		{
			// Set the hit result if we reach maxDistance on next step
			hitReasult.didImpact			= false;
			hitReasult.impactPosition		= currentPos;
			hitReasult.fractionTravelled	= distanceTravelled / maxDistance;
		}
	}

	return hitReasult;
}

Vector3 Terrain::SinWavePlane( float u, float v )
{
	Vector3 outPos	= Vector3( u, 0.f, v );
	outPos.y		= sinf( sqrtf( u*u + v*v ) / 10.f ) * m_maxHeight;

	return outPos;
}

Vector3 Terrain::GetVertexPositionUsingHeightMap( float u, float v )
{
	Vector3 outPos	= Vector3( u, 0.f, v );
	
	IntVector2 imageDimensions = m_heightMapImage->GetDimensions();
	u = RangeMapFloat( u, 0.f, (float)m_sampleSize.x, 0.f, (float)imageDimensions.x - 1.f );
	v = RangeMapFloat( v, 0.f, (float)m_sampleSize.y, 0.f, (float)imageDimensions.y - 1.f );
	u = ClampFloat( u, 0.f, (float)imageDimensions.x - 1.f );
	v = ClampFloat( v, 0.f, (float)imageDimensions.y - 1.f );

	float	rCol	= (float) m_heightMapImage->GetTexel( (int)u, (int)v ).r;
	outPos.y		= RangeMapFloat( rCol, 0.f, 255.f, 0.f, m_maxHeight );

	return outPos;
}

Vector3 Terrain::GiveQuadVertexForMyPositionAt( Vector2 myXZPosition, eTerrainQuadVetrex cornerVertex )
{
	// Get coords for Bottom Left Corner
	myXZPosition.x			= floorf( myXZPosition.x );
	myXZPosition.y			= floorf( myXZPosition.y );

	// Get corner cornerStep to add
	Vector2 cornerStep = Vector2::ZERO;
	switch ( cornerVertex )
	{
	case TERRAIN_QUAD_BOTTOM_LEFT:
		cornerStep = Vector2( 0.f, 0.f );
		break;
	case TERRAIN_QUAD_BOTTOM_RIGHT:
		cornerStep = Vector2( 1.f, 0.f );
		break;
	case TERRAIN_QUAD_TOP_LEFT:
		cornerStep = Vector2( 0.f, 1.f );
		break;
	case TERRAIN_QUAD_TOP_RIGHT:
		cornerStep = Vector2( 1.f, 1.f );
		break;
	default:
		GUARANTEE_OR_DIE( false, "Error: Invalid value for enum eTerrainQuadVetrex..!" );
		break;
	}

	// Get uv for that corner
	// m_xzScale 
	Vector2 uvBeforeScale	 = myXZPosition;
			uvBeforeScale	+= cornerStep;

	// Get world position from UVs
	Vector3 cornerPosition	 = GetVertexPositionUsingHeightMap( uvBeforeScale );

	return cornerPosition;
}

ChunkList Terrain::MakeChunksUsingSurfacePatch( std::function<Vector3( float, float )> SurfacePatch, IntVector2 maxChunkDimension )
{
	// LOGIC OVERVIEW:
	//
	// For each chunk
		// Get uvRangeMin & Max
		// Calculate new sampleSize
		// Use AddMeshFromSurfacePatch
			// Add it to ChunkList
	
	ChunkList chunks;

	// Get number of chunks
	IntVector2 numChunks;
	numChunks.x = (int) (m_sampleSize.x / maxChunkDimension.x);
	numChunks.y = (int) (m_sampleSize.y / maxChunkDimension.y);

	// If there are any leftover blocks after division
	if( m_sampleSize.x - (numChunks.x * maxChunkDimension.x) > 0 )
		numChunks.x++;
	if( m_sampleSize.y - (numChunks.y * maxChunkDimension.y) > 0 )
		numChunks.y++;
	
	for( int yChunk = 0; yChunk < numChunks.y; yChunk++ )
	{
		for( int xChunk = 0; xChunk < numChunks.x; xChunk++ )
		{
			// Inside a Chunk
			Vector2 bottomLeftUV	= Vector2( (float) xChunk * maxChunkDimension.x, (float) yChunk * maxChunkDimension.y );
			Vector2 topRightUV		= bottomLeftUV + maxChunkDimension;
			Vector2 centerUV		= ( bottomLeftUV + topRightUV ) * 0.5f;
			// In case if it overflows where there are no more tiles
			topRightUV.x			= ClampFloat( topRightUV.x, 0.f, (float)m_sampleSize.x );
			topRightUV.y			= ClampFloat( topRightUV.y, 0.f, (float)m_sampleSize.y );

			// Get new sample size for this chunk
			int			ssX			= (int) ( topRightUV.x - bottomLeftUV.x );
			int			ssY			= (int) ( topRightUV.y - bottomLeftUV.y );
			IntVector2	sampleSize	= IntVector2( ssX, ssY );

			// Get the pivot point for this Chunk
			Vector3		chunkCenterPos	= Vector3( centerUV.x, 0.f, centerUV.y );

			MeshBuilder chunkMB;
			chunkMB.Begin( PRIMITIVE_TRIANGES, true );
			chunkMB.AddMeshFromSurfacePatch( SurfacePatch, bottomLeftUV, topRightUV, sampleSize, RGBA_WHITE_COLOR );
			chunkMB.SetVertexPositionsRelativeTo( chunkCenterPos );
			chunkMB.End();

			// Create a renderable
			Transform chunkTransform	= Transform( chunkCenterPos, Vector3::ZERO, Vector3::ONE_ALL );
			chunkTransform.SetParentAs( &m_transform );
			m_renderable				= new Renderable( chunkTransform );

			Mesh *terrainMesh = chunkMB.ConstructMesh<Vertex_Lit>();
			m_renderable->SetBaseMesh( terrainMesh );
			Material *terrainMaterial = Material::CreateNewFromFile( "Data\\Materials\\terrain.material" );
			m_renderable->SetBaseMaterial( terrainMaterial );

			chunks.push_back( m_renderable );

			m_renderable = nullptr;
		}
	}

	return chunks;
}
