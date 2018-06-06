#pragma once
#include <vector>
#include "Engine/Math/Vector2.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////
// Standalone curve utility functions
//
// Note: Quadratic Bezier functions (only) specify an actual midpoint, not a Bezier guide point
/////////////////////////////////////////////////////////////////////////////////////////////////

template< typename T >
T EvaluateQuadraticBezier( const T& startPos, const T& actualCurveMidPos, const T& endPos, float t )
{
	T startEndHalfPos			= (startPos + endPos) * 0.5f;
	T addToGetMidControlPos		= actualCurveMidPos - startEndHalfPos;
	T midControlPos				= actualCurveMidPos + addToGetMidControlPos;

	T e_startToMid				= Interpolate( startPos, midControlPos, t );
	T f_midToEnd				= Interpolate( midControlPos, endPos, t );
	T p_eToF					= Interpolate( e_startToMid, f_midToEnd, t );

	return p_eToF;
}

template< typename T >
T EvaluateCubicBezier( const T& startPos, const T& guidePos1, const T& guidePos2, const T& endPos, float t )
{
	T e_startToGuidePos1			= Interpolate( startPos,				guidePos1,					t );
	T f_guidePos1ToGuidePos2		= Interpolate( guidePos1,				guidePos2,					t );
	T g_guidePos2ToEnd				= Interpolate( guidePos2,				endPos,						t );
	T q_eToF						= Interpolate( e_startToGuidePos1,		f_guidePos1ToGuidePos2,		t );
	T r_fToG						= Interpolate( f_guidePos1ToGuidePos2,	g_guidePos2ToEnd,			t );
	T p_qToR						= Interpolate( q_eToF,					r_fToG,						t );

	return p_qToR;
}

template< typename T >
T EvaluateCubicHermite( const T& startPos, const T& startVel, const T& endPos, const T& endVel, float t )
{
	T a		= startPos;
	T d		= endPos;
	T b		= a + ( startVel * 0.3333f );
	T c		= d - ( endVel   * 0.3333f );

	return EvaluateCubicBezier( a, b, c, d, t );
}


/////////////////////////////////////////////////////////////////////////////////////////////////
// CubicSpline2D
// 
// Cubic Hermite/Bezier spline of Vector2 positions / velocities
/////////////////////////////////////////////////////////////////////////////////////////////////
class CubicSpline2D
{
public:
			 CubicSpline2D() {}
	explicit CubicSpline2D( const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray=nullptr );
			~CubicSpline2D() {}

	// Mutators
	void			AppendPoint( const Vector2& position, const Vector2& velocity=Vector2::ZERO );
	void			AppendPoints( const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray=nullptr );
	void			InsertPoint( int insertBeforeIndex, const Vector2& position, const Vector2& velocity=Vector2::ZERO );
	void			RemovePoint( int pointIndex );
	void			RemoveAllPoints();
	void			SetPoint( int pointIndex, const Vector2& newPosition, const Vector2& newVelocity );
	void			SetPosition( int pointIndex, const Vector2& newPosition );
	void			SetVelocity( int pointIndex, const Vector2& newVelocity );
	void			SetCardinalVelocities( float tension=0.f, const Vector2& startVelocity=Vector2::ZERO, const Vector2& endVelocity=Vector2::ZERO );

	// Accessors
	int				GetNumPoints() const { return (int) m_positions.size(); }
	const Vector2	GetPosition( int pointIndex ) const;
	const Vector2	GetVelocity( int pointIndex ) const;
	int				GetPositions( std::vector<Vector2>& out_positions ) const;
	int				GetVelocities( std::vector<Vector2>& out_velocities ) const;
	Vector2			EvaluateAtCumulativeParametric( float t ) const;
	Vector2			EvaluateAtNormalizedParametric( float t ) const;

protected:
	std::vector<Vector2>	m_positions;
	std::vector<Vector2>	m_velocities;
};