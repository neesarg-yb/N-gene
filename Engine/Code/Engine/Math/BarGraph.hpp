#pragma once
#include <vector>
#include "Engine/Math/DoubleRange.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Mesh;
class AABB2;

struct DoubleAndVoidPointer
{
	double	value	= 0.0;
	void*	object	= nullptr;

	DoubleAndVoidPointer() {}
	DoubleAndVoidPointer( double value, void* objectPtr = nullptr )
	{
		this->value		= value;
		this->object	= objectPtr;
	}
};

class BarGraph
{
public:
	 BarGraph( int maxDataPoints );
	~BarGraph();

public:
	// Info about this graph
	int const								m_maxDataPoints;
	DoubleRange								m_valueRange;

private:
	// Data Points
	int										m_availableIndex = 0;		// Next Available index to replace the data with
	std::vector< DoubleAndVoidPointer >		m_dataPoints;				// Note: I'm treating it as an array of length m_maxDataPoints

public:
	void	AppendDataPoint( double const &value, void *objectPointer = nullptr );
	void	GetPreviousDataPoint( DoubleAndVoidPointer &outDataPoint, int skipCount = 0 );
	Mesh*	CreateVisualGraphMesh( AABB2 const &bounds );
};