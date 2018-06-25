#include "HeatMap2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtil.hpp"

HeatMap2D::HeatMap2D( const IntVector2& mapDimension, float initialHeatValue )
	: m_mapDimension( mapDimension )
	, m_initialHeatValue( initialHeatValue )
{
	// Set all individual heatValues to initialHeatValue
	int totalCells = m_mapDimension.x * m_mapDimension.y;

	for( int i = 0; i < totalCells; i++ )
	{
		m_heatPerGridCell.push_back( m_initialHeatValue );
	}
}

void HeatMap2D::SetHeat( float heatValue, const IntVector2& cellCoords )
{
	GUARANTEE_RECOVERABLE( cellCoords.x < m_mapDimension.x && cellCoords.y < m_mapDimension.y, std::string("HeatMap: SetHeat()'s cellCoords do not match with m_mapDimension..!") );

	int index = GetIndexFromColumnRowNumberForMatrixOfWidth( cellCoords.x, cellCoords.y, m_mapDimension.x );
	m_heatPerGridCell[ index ] = heatValue;
}

void HeatMap2D::AddHeat( float heatAddition, const IntVector2& cellCoords )
{
	GUARANTEE_RECOVERABLE( cellCoords.x < m_mapDimension.x && cellCoords.y < m_mapDimension.y, std::string("HeatMap: AddHeat()'s cellCoords do not match with m_mapDimension..!") );

	int index = GetIndexFromColumnRowNumberForMatrixOfWidth( cellCoords.x, cellCoords.y, m_mapDimension.x );
	m_heatPerGridCell[ index ] += heatAddition;
}

float HeatMap2D::GetHeat( const IntVector2& cellCoords ) const
{
	GUARANTEE_RECOVERABLE( cellCoords.x < m_mapDimension.x && cellCoords.y < m_mapDimension.y, std::string("HeatMap: GetHeat()'s cellCoords do not match with m_mapDimension..!") );

	int index = GetIndexFromColumnRowNumberForMatrixOfWidth( cellCoords.x, cellCoords.y, m_mapDimension.x );
	return m_heatPerGridCell[ index ];
}