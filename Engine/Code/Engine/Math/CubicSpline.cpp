#include "CubicSpline.hpp"

CubicSpline2D::CubicSpline2D( const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray/* =nullptr */ )
{
	for( int i = 0; i < numPoints; i++ )
	{
		m_positions.push_back( positionsArray[i] );
		
		if( velocitiesArray != nullptr )
			m_velocities.push_back( velocitiesArray[i] );
		else
			m_velocities.push_back( Vector2::ZERO );
	}
}


// Mutators
void CubicSpline2D::AppendPoint( const Vector2& position, const Vector2& velocity/* =Vector2::ZERO */ )
{
	m_positions.push_back( position );
	m_velocities.push_back( velocity );
}

void CubicSpline2D::AppendPoints( const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray/* =nullptr */ )
{
	for( int i = 0; i < numPoints; i++ )
	{
		m_positions.push_back( positionsArray[i] );

		if( velocitiesArray != nullptr )
			m_velocities.push_back( velocitiesArray[i] );
		else
			m_velocities.push_back( Vector2::ZERO );
	}
}

void CubicSpline2D::InsertPoint( int insertBeforeIndex, const Vector2& position, const Vector2& velocity/* =Vector2::ZERO */ )
{
	std::vector<Vector2>::iterator insertIterator = m_positions.begin() + insertBeforeIndex;
	m_positions.insert( insertIterator, position );

	insertIterator = m_velocities.begin() + insertBeforeIndex;
	m_velocities.insert( insertIterator, velocity );
}

void CubicSpline2D::RemovePoint( int pointIndex )
{
	std::vector<Vector2>::iterator eraseIterator = m_positions.begin() + pointIndex;
	m_positions.erase( eraseIterator );

	eraseIterator = m_velocities.begin() + pointIndex;
	m_velocities.erase( eraseIterator );
}

void CubicSpline2D::RemoveAllPoints()
{
	m_positions.erase( m_positions.begin(), m_positions.end() );
	m_velocities.erase( m_velocities.begin(), m_velocities.end() );
}

void CubicSpline2D::SetPoint( int pointIndex, const Vector2& newPosition, const Vector2& newVelocity )
{
	m_positions[ pointIndex ]  = newPosition;
	m_velocities[ pointIndex ] = newVelocity;
}

void CubicSpline2D::SetPosition( int pointIndex, const Vector2& newPosition )
{
	m_positions[ pointIndex ] = newPosition;
}

void CubicSpline2D::SetVelocity( int pointIndex, const Vector2& newVelocity )
{
	m_velocities[ pointIndex ] = newVelocity;
}

void CubicSpline2D::SetCardinalVelocities( float tension/* =0.f */, const Vector2& startVelocity/* =Vector2::ZERO */, const Vector2& endVelocity/* =Vector2::ZERO */ )
{
	m_velocities.erase( m_velocities.begin(), m_velocities.end() );
	float velocityMultiplier = 1.f - tension;

	// Set startVelocity
	m_velocities.push_back( startVelocity * velocityMultiplier );

	// For each points between start and end
	for( unsigned int i = 1; i < m_positions.size() - 1; i++ )
	{
		Vector2 previousPointPos = m_positions[ i-1 ];
		Vector2 nextPointPos	 = m_positions[ i+1 ];
		Vector2 newVelocity		 = ( nextPointPos - previousPointPos ) * 0.5f;

		m_velocities.push_back( newVelocity * velocityMultiplier );
	}

	// Set endVelocity
	m_velocities.push_back( endVelocity * velocityMultiplier );
}

// Accessors
const Vector2 CubicSpline2D::GetPosition( int pointIndex ) const
{
	return m_positions[ pointIndex ];
}

const Vector2 CubicSpline2D::GetVelocity( int pointIndex ) const
{
	return m_velocities[ pointIndex ];
}

int CubicSpline2D::GetPositions( std::vector<Vector2>& out_positions ) const
{
	out_positions = m_positions;

	return (int) m_positions.size();
}

int	CubicSpline2D::GetVelocities( std::vector<Vector2>& out_velocities ) const
{
	out_velocities = m_velocities;

	return (int) m_velocities.size();
}

Vector2	CubicSpline2D::EvaluateAtCumulativeParametric( float t ) const
{
	int curveNumber		= (int) t;
	float local_t		= t - curveNumber;

	Vector2 startPos	= m_positions [ curveNumber ];
	Vector2 startVel	= m_velocities[ curveNumber ];
	Vector2 endPos		= m_positions [ curveNumber+1 ];
	Vector2 endVel		= m_velocities[ curveNumber+1 ];

	return EvaluateCubicHermite( startPos, startVel, endPos, endVel, local_t );
}

Vector2	CubicSpline2D::EvaluateAtNormalizedParametric( float t ) const				// TODO: Failing MP1A6_UnitTest for t = 0.125f; o/p should be ( 3.5, 2.25 ). Fix that!
{
	int		totalCurves	= (int) m_positions.size() - 1;
	float	cumulativeT	= totalCurves * t;
	int		curveNumber	= (int) cumulativeT;
	float	local_t		= t - curveNumber;

	Vector2 startPos	= m_positions [ curveNumber ];
	Vector2 startVel	= m_velocities[ curveNumber ];
	Vector2 endPos		= m_positions [ curveNumber+1 ];
	Vector2 endVel		= m_velocities[ curveNumber+1 ];

	Vector2 toReturn = EvaluateCubicHermite( startPos, startVel, endPos, endVel, local_t );

	return toReturn;
}