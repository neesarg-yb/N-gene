#pragma once
#include <vector>
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector3.hpp"

class HeatMap3D
{
public:
	 HeatMap3D( IntVector3 const &mapDimension, float initialHeatValue );
	~HeatMap3D() { }

public:
	std::vector< float >	m_heatPerCell;
	IntVector3 const		m_mapDimension;
	float const				m_initialHeatValue;

public:
	void	SetHeat( float heatValue,		IntVector3 const &cellCoord );
	void	AddHeat( float heatAddition,	IntVector3 const &cellCoord );
	float	GetHeat( IntVector3 const &cellCoord ) const;

private:
	int		GetIndexFromXYZCoordForTowerHavingXZDimension( int x, int y, int z , IntVector2 xzDimension ) const;
};