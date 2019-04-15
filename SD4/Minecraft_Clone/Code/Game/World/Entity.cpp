#pragma 
#include "Entity.hpp"

Entity::Entity( Clock *parentClock )
	: m_clock( parentClock )
{

}

Entity::~Entity()
{

}

void Entity::Update()
{
	float deltaSeconds = (float) m_clock.GetFrameDeltaSeconds();

	// Compute willpower forces
	Vector2 willpowerForceXY = m_willpowerForce.IgnoreZ();
	float	willpowerForceZ	 = m_willpowerForce.z;
	
	// Compute Friction
	Vector2	frictionXY	= m_velocity.IgnoreZ() * m_frictionScale * -1.f;
	float	frictionZ	= m_velocity.z * m_frictionScale * -1.f;

	// Add up Willpower & Friction
	Vector2 totalForceXY = willpowerForceXY + frictionXY;
	float totalForceZ	 = willpowerForceZ + frictionZ;

	// Add up Gravity..
	if( m_physicsMode == PHYSICS_WALK )
		totalForceZ -= m_gravity;

	// Speed prior to applying forces
	float speedBeforeXY = m_velocity.IgnoreZ().GetLength();

	// Compute the displacement to travel this frame
	Vector2	deltaVelocityXY	= totalForceXY * deltaSeconds;
	float	deltaVelocityZ	= totalForceZ  * deltaSeconds;

	// Apply forces & Update velocity
	m_velocity += Vector3( deltaVelocityXY.x, deltaVelocityXY.y, deltaVelocityZ );

	// Speed after applying forces
	float speedAfterXY = m_velocity.IgnoreZ().GetLength();

	// Constrain velocity based on speed limits
	if( speedAfterXY > speedBeforeXY )
	{
		float willpowerSpeedLimitXY = m_walkingMaxSpeedXY;
		if( m_physicsMode != PHYSICS_WALK )
			willpowerSpeedLimitXY = m_flyingMaxSpeedXY;

		// Maximum of two
		float speedLimit = (speedBeforeXY > willpowerSpeedLimitXY) ? speedBeforeXY : willpowerSpeedLimitXY;
		m_velocity.LimitLengthTo( speedLimit );
	}

	// Move the entity
	m_position += m_velocity * deltaSeconds;
}
