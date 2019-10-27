#pragma once
#include "Trajectory.hpp"
#include "Engine/Math/MathUtil.hpp"

Vector2 Trajectory::Evaluate( float gravity, Vector2 launchVelocity, float time )
{
	float v		= launchVelocity.GetLength();
	float theta	= launchVelocity.GetOrientationDegrees();

	return Evaluate( gravity, v, theta, time );
}

Vector2 Trajectory::Evaluate( float gravity, float launchSpeed, float launchAngleDegree, float time )
{
	// Formula:
	//
	// f(t) = ( v*cos(theta)*t , -0.5*(g)*(t*t) + v*sin(theta)*t );
	//
	//					v		= launch speed
	//					g		= gravity ( positive )
	//					theta	= launchAngle
	//					t		= time

	float x		= launchSpeed * CosDegree( launchAngleDegree ) * time;
	float y		= ( -0.5f * gravity * (time*time) ) + ( launchSpeed * SinDegree( launchAngleDegree ) * time );

	return Vector2( x, y );
}

float Trajectory::GetMinimumLaunchSpeed( float gravity, float distance, float angleDegrees /* = 45.f */ )
{
	float speedSquared = (gravity * distance) / SinDegree(2*angleDegrees);

	return sqrtf( speedSquared );
}

bool Trajectory::GetLaunchAngles( Vector2& out_angles, float gravity, float launchSpeed, float distance, float height /* = 0.f */ )
{
	// x = v*cos( theta )*t;
	// y = -0.5*g*(t^2) + v*sin( theta )*t
	//
	// t = x / (v*cos( theta ))
	//
	// Now, get y in terms of tan( theta )'s quadratic equation.
	// And then,
	// To solve equation:  a*tan^2( theata ) + b*tan( theta ) + c = 0

	float a = ( gravity * distance * distance ) / ( 2.f * launchSpeed * launchSpeed );
	float b = -1.f * distance;
	float c = height + a;

	Vector2	tanThetaSolutions;
	bool	equationHasRealRoots = SolveQuadraticEquation( tanThetaSolutions, a, b, c );

	if( !equationHasRealRoots )
		return false;

	out_angles.x = RadianToDegree( atan2f( tanThetaSolutions.x, 1.f ) );
	out_angles.y = RadianToDegree( atan2f( tanThetaSolutions.y, 1.f ) );

	// Sort the out_angles
	if( out_angles.x > out_angles.y )
	{
		float temp		= out_angles.x;
		out_angles.x	= out_angles.y;
		out_angles.y	= temp;
	}
	
	return true;
}

float Trajectory::GetMaxHeight( float gravity, float launch_speed, float distance )
{
	// First, get angle for which y gets maximum
	//		it comes when we assume that d(y)/d(tanTheta) will be ZERO
	//		i.e. change in y compared to change in tanTheta is zero, at the target distance, for max reachable height
	float c			= -1.f * (gravity * distance * distance) / (2.f * launch_speed * launch_speed);
	float y			= -0.5f * distance / c;
	float radians	= atan2f( y, 1.f );
 	
	// Now use that theta into y equation of Trajectory, to get the height at that theta
	float tanTheta	= tanf( radians );
	float maxHeight = c + (c * tanTheta * tanTheta) + (distance * tanTheta);
	
	return maxHeight;
}

Vector2 Trajectory::GetLaunchVelocity( float gravity, float apex_height, float distance, float height )
{
	// When projectile reaches the apex_height, its dy/dt = 0
	// Use it to find tApex
	// Use tApex to find Vy
	float tApex		= sqrtf( 2.f * apex_height / gravity );
	float Vy		= gravity * tApex;

	// Now to get total t when projectile reaches the distance at given height
	//	Solv: -0.5f*gravity*(t^2) + Vy*t - height = 0
	Vector2 t_values;
	float	a			= -0.5f * gravity;
	bool	realRoots	= SolveQuadraticEquation( t_values, a, Vy, -height );
	
	// Use second value of t to get Vx
	// Why second value? B/C in the case where projectile reaches the given height two times in single launch,
	//		It is the second time of the t, where projectile will be at end-distance
	GUARANTEE_RECOVERABLE( realRoots, "Error GetLaunchVelocity: can't find realRoots for t, in trajectory equation.." );
	float Vx = distance / t_values.y;

	return Vector2( Vx, Vy );
}