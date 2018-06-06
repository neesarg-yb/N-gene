#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Trajectory
{
public:
	static Vector2	Evaluate( float gravity, Vector2 launchVelocity, float time );
	static Vector2	Evaluate( float gravity, float launchSpeed, float launchAngleDegree, float time );
	static float	GetMinimumLaunchSpeed( float gravity, float distance, float angleDegrees = 45.f );
	static bool		GetLaunchAngles( Vector2& out_angles, float gravity, float launchSpeed, float distance, float height = 0.f );
	static float	GetMaxHeight( float gravity, float launch_speed, float distance );
	static Vector2	GetLaunchVelocity( float gravity, float apex_height, float distance, float height );
};