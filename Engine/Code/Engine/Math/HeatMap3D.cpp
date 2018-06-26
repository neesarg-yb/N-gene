#pragma once
#include "HeatMap3D.hpp"
#include "Engine/Core/EngineCommon.hpp"

HeatMap3D::HeatMap3D( IntVector3 const &mapDimension, float initialHeatValue )
	: m_mapDimension( mapDimension )
	, m_initialHeatValue( initialHeatValue )
{
	// Set all individual heatValues to m_initialiHeatValue
	int totalCells = ( m_mapDimension.x * m_mapDimension.z ) * m_mapDimension.y;		// ( width * length ) * height

	for( int i = 0; i < totalCells; i++ )
		m_heatPerCell.push_back( m_initialHeatValue );
}

void HeatMap3D::SetHeat( float heatValue, IntVector3 const &cellCoord )
{
	// Check if cellCoord isn't overflowing Map Dimension
	bool dimensionsMatches	=	( cellCoord.x < m_mapDimension.x )
							&&	( cellCoord.y < m_mapDimension.y )
							&&	( cellCoord.z < m_mapDimension.z );
	GUARANTEE_OR_DIE( dimensionsMatches, "Error!! HeatMap::SetHeat( ) - Passed cellCord exceeds the heatmap size..!" );
	
	// Set Heat
	IntVector2 xzDimension	= IntVector2( m_mapDimension.x, m_mapDimension.z );
	int index				= GetIndexFromXYZCoordForTowerHavingXZDimension( cellCoord.x, cellCoord.y, cellCoord.z, xzDimension );
	m_heatPerCell[ index ]	= heatValue;
}

void HeatMap3D::AddHeat( float heatAddition, IntVector3 const &cellCoord )
{
	// Check if cellCoord isn't overflowing Map Dimension
	bool dimensionsMatches	=	( cellCoord.x < m_mapDimension.x )
							&&	( cellCoord.y < m_mapDimension.y )
							&&	( cellCoord.z < m_mapDimension.z );
	GUARANTEE_OR_DIE( dimensionsMatches, "Error!! HeatMap::SetHeat( ) - Passed cellCord exceeds the heatmap size..!" );

	// Add Heat
	IntVector2 xzDimension	 = IntVector2( m_mapDimension.x, m_mapDimension.z );
	int index				 = GetIndexFromXYZCoordForTowerHavingXZDimension( cellCoord.x, cellCoord.y, cellCoord.z, xzDimension );
	m_heatPerCell[ index ]	+= heatAddition;
}

float HeatMap3D::GetHeat( IntVector3 const &cellCoord ) const
{
	// Check if cellCoord isn't overflowing Map Dimension
	bool dimensionsMatches	=	( cellCoord.x < m_mapDimension.x )
							&&	( cellCoord.y < m_mapDimension.y )
							&&	( cellCoord.z < m_mapDimension.z );
	GUARANTEE_OR_DIE( dimensionsMatches, "Error!! HeatMap::SetHeat( ) - Passed cellCord exceeds the heatmap size..!" );

	// Get Heat
	IntVector2 xzDimension	= IntVector2( m_mapDimension.x, m_mapDimension.z );
	int index				= GetIndexFromXYZCoordForTowerHavingXZDimension( cellCoord.x, cellCoord.y, cellCoord.z, xzDimension );
	
	return m_heatPerCell[ index ];
}

int HeatMap3D::GetIndexFromXYZCoordForTowerHavingXZDimension( int x, int y, int z , IntVector2 xzDimension ) const
{
	uint numBlocksInALayer	= xzDimension.x * xzDimension.y;
	uint idx				= ( x )							// rowIdx
							+ ( z * xzDimension.x )			// columnIdx * width
							+ ( y * numBlocksInALayer );	// hightIdx  * blocksInALayer

	return idx;
}
