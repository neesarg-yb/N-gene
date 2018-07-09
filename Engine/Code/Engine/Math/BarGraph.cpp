#pragma once
#include "BarGraph.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"

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

Mesh* BarGraph::CreateVisualGraphMesh( AABB2 const &bounds, Rgba const &barColor /* = RGBA_BLUE_COLOR */ )
{
	MeshBuilder graphMB;
	graphMB.Begin( PRIMITIVE_TRIANGES, true );

	// We'll need it to draw each bar
	double rangeLength = m_valueRange.GetRangeLength();

	float	barWidth			= ( bounds.maxs.x - bounds.mins.x ) / m_maxDataPoints;
	float	barHeight			= ( bounds.maxs.y - bounds.mins.y );
	Vector2 barHalfDimension	= Vector2( barWidth * 0.5f, barHeight * 0.5f );
	Vector2 centerOfFirstBar	= bounds.mins + barHalfDimension;

	// For each dataPoints add new bar/quad to MeshBuilder
	for( int i = 0; i < m_maxDataPoints; i++ )
	{
		// Get the dataPoint
		int skipIdx = (m_maxDataPoints - 1) - i;
		
		DoubleAndVoidPointer dataPoint;
		GetPreviousDataPoint( dataPoint, skipIdx );

		float	valueAsFraction		= (float)( dataPoint.value / rangeLength );
		Vector2 centerOfThisBar		= centerOfFirstBar + Vector2( barWidth * i, 0.f );
		Vector2 dimensionOfThisBar	= Vector2( barWidth, barHeight * valueAsFraction );

		AABB2	thisBarsBound		= AABB2( centerOfThisBar, dimensionOfThisBar.x, dimensionOfThisBar.y );
		graphMB.AddPlane( dimensionOfThisBar, centerOfThisBar.GetAsVector3(), barColor );
	}

	graphMB.End();

	return graphMB.ConstructMesh<Vertex_Lit>();
}

