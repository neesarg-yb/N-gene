#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Game/Polygon2.hpp"
#include "Engine/Core/Rgba.hpp"

class Entity {
public:
	Entity();
	virtual ~Entity();

//	Vector2 m_position;								!! PRIVATE MEMBER !!
	float	m_markForDelete = false;
	float   m_health;

	Vector2 m_velocity;								// speed of the object
	float	m_orientationAngleInDegree;				// Entity will move in this direction
	float	m_rotationSpeedInDegreePerSeconds;		// Entity's rotation speed
	float	m_rotationSpeedMultiplier = 1.f;		// Can be used as a sign to make object rotate clockwise or anti-clockwise

	Disc2	m_visualDisc2;							// It represents visual boundary of an object
	Disc2	m_collosionDisc2;						// It represents body collider boundary
	
	virtual void Render();
	virtual void Update( float deltaSeconds );
	
	Vector2 GetPosition() const;					// Get Position
	void	SetPosition( const Vector2& newPosition );	// Changing position, will also change m_VisualDisc, m_CollisionDisc positions
	float	GetSpeed() const;						// Get speed from velocity Vector2
	float	GetVelocityDirectionInDegree() const;	// Get angle of velocity Vector2 in degree
	
	void	SetVelocityDirectionInDegree(float speed , float degree);		// Set angle of velocity Vector2 in degree


	void CheckAndWrapAround();						// The wrap around function
	virtual void drawDebugInformations();					// Draws info for Debug Mode
	void ReduceAHealth();

private:
	Vector2 m_position;								// position of Entity object
	void	SetSpeed(float speed);					// Set speed using velocity Vector2

};