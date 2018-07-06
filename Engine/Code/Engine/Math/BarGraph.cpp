#pragma once
#include "BarGraph.hpp"
#include "Engine/Math/MathUtil.hpp"

BarGraph::BarGraph( int maxDataPoints )
	: m_maxDataPoints( maxDataPoints )
{
	double dummyValue = 0.0;

	// Fill the vector with dummy data points
	for( int i = 0; i < m_maxDataPoints; i++ )
		m_dataPoints.push_back( DoubleAndVoidPointer( dummyValue ) );

	// Set the dummy range
	m_valueRange = DoubleRange( dummyValue );
}

BarGraph::~BarGraph()
{

}

void BarGraph::AppendDataPoint( double const &value, void *objectPointer /*= nullptr */ )
{
	// Adjust range if needs to be
	m_valueRange.ChangeRangeToInclude( value );

	// If index is out of range
	m_availableIndex = m_availableIndex % m_maxDataPoints;
	
	// Replace the data point
	m_dataPoints[ m_availableIndex ] = DoubleAndVoidPointer( value, objectPointer );

	// Increment the available index
	m_availableIndex++;
}

void BarGraph::GetPreviousDataPoint( DoubleAndVoidPointer &outDataPoint, int skipCount /* = 0 */ )
{
	int actualIndex = (m_availableIndex - 1) - skipCount;
	actualIndex		= ModuloNonNegative( actualIndex, m_maxDataPoints );
	
	outDataPoint = m_dataPoints[ actualIndex ];
}

