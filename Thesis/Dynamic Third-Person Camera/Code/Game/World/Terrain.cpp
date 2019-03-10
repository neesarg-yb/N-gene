#pragma once
#include "Terrain.hpp"
#include "Engine/Core/Ray3.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/ContactPoint.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Profiler/Profiler.hpp"

Terrain::Terrain( Vector3 spawnPosition, IntVector2 gridSize, float maxHeight, std::string heightMapImagePath, eTerrainMaterial materialFile )
	: m_maxHeight( maxHeight )
	, m_sampleSize( gridSize )
{
	switch( materialFile )
	{
	case TERRAIN_GRIDLINES:
		m_materialFilePath = "Data\\Materials\\terrain_gridlines.material";
		break;
	case TERRAIN_GRASS:
		m_materialFilePath = "Data\\Materials\\terrain_grass.material";
		break;
	default:
		m_materialFilePath = "Data\\Materials\\terrain_gridlines.material";
		break;
	}

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
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

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

Vector3 Terrain::GetNormalForMyPositionAt( Vector2 myXZPosition ) const
{
	// Get Position on Terrain
	Vector3 terrainWorldPos	= m_transform.GetWorldPosition();
	Vector2 posOnTerrain	= myXZPosition - Vector2( terrainWorldPos.x, terrainWorldPos.z );

	// Get Position of four corners
	Vector3 bottomLeftPos	= GiveQuadVertexForMyPositionAt( posOnTerrain, TERRAIN_QUAD_BOTTOM_LEFT  );
	Vector3 bottomRightPos	= GiveQuadVertexForMyPositionAt( posOnTerrain, TERRAIN_QUAD_BOTTOM_RIGHT );
	Vector3 topLeftPos		= GiveQuadVertexForMyPositionAt( posOnTerrain, TERRAIN_QUAD_TOP_LEFT	 );
	Vector3 topRightPos		= GiveQuadVertexForMyPositionAt( posOnTerrain, TERRAIN_QUAD_TOP_RIGHT	 );

	// Four Edge Directions
	//
	// t.l.       t.r.
	//   ^---->--^   
	//   |       |   
	//   |       |   
	//   |       |   
	//   ----->---   
	// b.l.       b.r.
	Vector3 botLeftToBotRight  = bottomRightPos	- bottomLeftPos;
	Vector3 topLeftToTopRight  = topRightPos	- topLeftPos;
	Vector3 botLeftToTopLeft   = topLeftPos		- bottomLeftPos;
	Vector3 botRightToTopRight = topRightPos	- bottomRightPos;

	// Four Corners' Normal
	Vector3 botLeftNormal  = Vector3::CrossProduct( botLeftToTopLeft, botLeftToBotRight ).GetNormalized();
	Vector3 topLeftNormal  = Vector3::CrossProduct( topLeftToTopRight, botLeftToTopLeft * -1.f ).GetNormalized();
	Vector3 topRightNormal = Vector3::CrossProduct( topLeftToTopRight, botRightToTopRight * -1.f ).GetNormalized();
	Vector3 botRightNormal = Vector3::CrossProduct( botLeftToBotRight * -1.f, botRightToTopRight ).GetNormalized();

	// Bipolar Interpolation of normals
	Vector2 xzFraction;
	xzFraction.x = fmodf( posOnTerrain.x, 1.f );
	xzFraction.y = fmodf( posOnTerrain.y, 1.f );

	Vector3 topInterpNormal		= Interpolate( topLeftNormal, topRightNormal, xzFraction.x );
	Vector3 bottomInterpNormal	= Interpolate( botLeftNormal, botRightNormal, xzFraction.x );
	Vector3 interpNormal		= Interpolate( bottomInterpNormal, topInterpNormal, xzFraction.y );

	return interpNormal;
}

float Terrain::GetYCoordinateForMyPositionAt( Vector2 myXZPosition, float yOffset /* = 0.f */ ) const
{
	// Note: myXZPosition is in World Space
	// You'll get corner's position in Terrain Space by passing corresponding local XZCoord
	// After that you need to convert it to world space before three lerp(s)

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
	float	myHeight		= Interpolate( heightBottom,	heightTop,		xzFraction.y );

	return myHeight + yOffset;
}

Vector3 Terrain::Get3DCoordinateForMyPositionAt( Vector2 myXZPosition, float yOffset /* = 0.f */ ) const
{
	float yCoord = GetYCoordinateForMyPositionAt( myXZPosition, yOffset );

	return Vector3( myXZPosition.x, yCoord, myXZPosition.y );
}

Plane3 Terrain::GetPlaneForMyPositionAt( Vector2 myXZPosition ) const
{
	Vector3 pointOnTerrain	= Get3DCoordinateForMyPositionAt( myXZPosition );
	Vector3 normalAtPoint	= GetNormalForMyPositionAt( myXZPosition );

	return Plane3( normalAtPoint, pointOnTerrain );
}

RaycastResult Terrain::Raycast( Vector3 const &startPosition, Vector3 direction, float const maxDistance, float const accuracy )
{
	PROFILE_SCOPE_FUNCTION();

	float const sampleSize = 1.f;
	// Logic:
	//
	//	Start moving in the direction from start point, until you're on the other side of the terrain
	//	Once you're on the other side,
	//		Start doing the Ray March until we reach the accuracy
	float distanceFromTerrain	= startPosition.y - GetYCoordinateForMyPositionAt(startPosition.x, startPosition.z);
	float aboveBelowSign		= GetSign( distanceFromTerrain );
	float const startSign		= aboveBelowSign; // We'll use it to make sure that the impactPosition we suggest is on the other side of startPosition
	
	// Ray
	Ray3 ray = Ray3( startPosition, direction );

	Vector3 position;
	bool	didImpact = false;
	for ( float t = 0.f; (t <= maxDistance) && (didImpact == false); t = fminf( t + sampleSize, maxDistance ) )
	{
		position	= ray.Evaluate( t );
		distanceFromTerrain	= position.y - GetYCoordinateForMyPositionAt( position.x, position.z );

		// If we just crossed the terrain
		if( aboveBelowSign != GetSign( distanceFromTerrain ) )
		{
			didImpact		= true;		// i.e. we did impact in this step
			aboveBelowSign *= -1.f;		// thus this sign also gets changed!
			
			// Assuming that you are now above the terrain
			Vector3 abovePoint = position;				// A point above the terrain
			Vector3 belowPoint = position - direction;	// A point below the terrain
										
			// But in reality, if you're now below the terrain
			if( aboveBelowSign == -1.f )
				std::swap( abovePoint, belowPoint );

			// Ray March until appropriate accuracy & until we're on the same side of the startPosition
			while ( abs(distanceFromTerrain) > accuracy || (aboveBelowSign == startSign) )
			{
				// Set Middle Point as our new position
				position = (abovePoint + belowPoint) * 0.5f;

				// Check distance from Terrain
				distanceFromTerrain = position.y - GetYCoordinateForMyPositionAt( position.x, position.z );
				aboveBelowSign		= GetSign( distanceFromTerrain );

				// Change one of these two points to Middle Point
				if( aboveBelowSign == 1.f )
					abovePoint = position;
				else
					belowPoint = position;
			}
		}

		// To terminate once we reach to max distance
		if( t == maxDistance )
			break;
	}
	
	// To check if the impact point is inside the terrain's bounds
	bool impactInsideTheBounds	= false;
	Vector2 positionXZ			= Vector2( position.x, position.z );
	AABB2	terrainBoundsXZ		= AABB2( m_worldBounds.mins.x, m_worldBounds.mins.z, m_worldBounds.maxs.x, m_worldBounds.maxs.z );
	if( terrainBoundsXZ.IsPointInside( positionXZ ) )
		impactInsideTheBounds = true;

	// If it did not impact & did impact but not inside the bounds of the terrain,
	if( didImpact == false || impactInsideTheBounds == false )
		return RaycastResult( position );			// Returns as if it did not hit!
	else
	{
		Vector2 impactPositionXZ	= Vector2( position.x, position.z );
		float	distanceTravelled	= ( startPosition - position ).GetLength();

		// Impact point
		ContactPoint impactPoint;
		impactPoint.position		= position;
		impactPoint.normal			= GetNormalForMyPositionAt( impactPositionXZ );
		
		// Fraction traveled
		float fractionTravelled		= distanceTravelled / maxDistance;

		// Return the result
		return RaycastResult( impactPoint.position, impactPoint.normal, fractionTravelled );
	}
}

Vector3 Terrain::CheckCollisionWithSphere( Vector3 const &center, float radius, bool &outIsColliding ) const
{
	float yCoordinateOnTerrain = GetYCoordinateForMyPositionAt( center.x, center.z );
	if( center.y - yCoordinateOnTerrain >= radius )
	{
		// Early out if the sphere is above the terrain-surface
		outIsColliding = false;
		return center;
	}
	else
	{
		// Sphere is colliding with terrain
		outIsColliding = true;

		// Get the Plane, so we can push the sphere in its normal
		Plane3 planeUnderCenter = GetPlaneForMyPositionAt( center.x, center.z );
		float distanceFromPlane = planeUnderCenter.GetDistanceFromPoint( center );

		float pushDistance	= radius - distanceFromPlane;
		Vector3 newCenter	= center + (planeUnderCenter.normal * pushDistance);

		return newCenter;
	}
}

Vector3 Terrain::SinWavePlane( float u, float v )
{
	Vector3 outPos	= Vector3( u, 0.f, v );
	outPos.y		= sinf( sqrtf( u*u + v*v ) / 10.f ) * m_maxHeight;

	return outPos;
}

Vector3 Terrain::GetVertexPositionUsingHeightMap( float u, float v ) const
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

Vector3 Terrain::GiveQuadVertexForMyPositionAt( Vector2 myXZPosition, eTerrainQuadVetrex cornerVertex ) const
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
			Material *terrainMaterial = Material::CreateNewFromFile( m_materialFilePath.c_str() );
			m_renderable->SetBaseMaterial( terrainMaterial );

			chunks.push_back( m_renderable );

			m_renderable = nullptr;
		}
	}

	return chunks;
}

// Vector3 Terrain::CheckCollisionWithSphere( Vector3 const &center, float radius, bool &outIsColliding ) const
// {
// 	// Get points on the radius of the sphere
// 	Vector2 xzPointsOnSphere[10];
// 	GetPointsOnCircle2D( Vector2(center.x, center.z), radius, 5, xzPointsOnSphere );
// 	GetPointsOnCircle2D( Vector2(center.x, center.z), radius * 0.5f, 5, &xzPointsOnSphere[5] );
// 
// 	// From these points we'll check about the collision
// 	Vector3 fetchPlanePoints[11];
// 
// 	// One of them being underneath the center
// 	fetchPlanePoints[10] = center;
// 
// 	// And from other points inside the sphere
// 	for( int i = 0; i < 10; i++ )
// 		fetchPlanePoints[i] = Vector3( xzPointsOnSphere[i].x, center.y, xzPointsOnSphere[i].y );
// 
// 	// Get maximum pushDistance
// 	float	pushDistance = 0.f;
// 	Vector3 pushDirection;
// 	for( int i = 0; i < 11; i++ )
// 	{
// 		Plane3 planeUnderneath	 = GetPlaneForMyPositionAt( fetchPlanePoints[i].x, fetchPlanePoints[i].z );
// 		float  distanceFromPlane = planeUnderneath.GetDistanceFromPoint( center );
// 
// 		float pushDistForThisPlane = radius - distanceFromPlane;
// 		if( pushDistForThisPlane > pushDistance )
// 		{
// 			pushDistance	= pushDistForThisPlane;
// 			pushDirection	= planeUnderneath.normal;
// 		}
// 	}
// 
// 	if( pushDistance == 0.f )
// 	{
// 		outIsColliding = false;
// 		return center;
// 	}
// 	else
// 	{
// 		outIsColliding = true;
// 
// 		// Push the center out of that plane
// 		Vector3 newCenter  = center + (pushDirection * pushDistance);
// 		return  newCenter;
// 	}
// }
