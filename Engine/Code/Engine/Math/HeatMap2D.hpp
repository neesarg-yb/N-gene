#pragma once

#include <vector>
#include "Engine/Math/IntVector2.hpp"

class HeatMap2D
{
public:
	HeatMap2D( const IntVector2& mapDimension, float initialHeatValue );

public:
	std::vector< float >	m_heatPerGridCell;
	const IntVector2		m_mapDimension;
	const float				m_initialHeatValue;

	void	SetHeat( float heatValue,		const IntVector2& cellCoords );
	void	AddHeat( float heatAddition,	const IntVector2& cellCoords );
	float	GetHeat( const IntVector2& cellCoords ) const;

private:

};