#include "Ship.hpp"

Ship::Ship()
{
	// Set all member variables of Entity class
	m_position = Vector2(500.f, 500.f);
	SetSpeed(0.f);
	SetVelocityDirectionInDegree(0.f , 90.f);
	m_orientationAngleInDegree = 90.f;
	m_visualDisc2 = Disc2(m_position, 30.f);
	m_collosionDisc2 = Disc2(m_position, 10.f);
	m_rotationSpeedInDegreePerSeconds = 180.f;

	// Set all member variables of Ship class
	m_thrustButtonPressed = false;
}

Ship::~Ship()
{

}

void Ship::Update(float deltaTime) {
	CheckAndWrapAround();														// First thing first: check for wrap around

	// Update position and Discs
	m_position += (deltaTime * m_velocity);
	m_visualDisc2.center = m_position;
	m_collosionDisc2.center = m_position;
}

void Ship::drawShip() const {
	// Draw Ship
	g_theRenderer->DrawFromVertexArray(m_shipVertexPoint, 5, m_position, m_orientationAngleInDegree, m_visualDisc2.radius);

	// Draw flames
	if(m_thrustButtonPressed == true) {
		Rgba whiteColor = Rgba(255, 255, 255, 255);
		
		float flamesEndX = MathUtil::GetRandomFloatInRange(-5, -3) / 8.f;		// Get Flame's Random End's x-axis
		Vector2 flameEndVertex = Vector2(flamesEndX, 0.f);
		Vector2 flameVertexes[3] = {m_shipVertexPoint[2], m_shipVertexPoint[3], flameEndVertex};
		
		g_theRenderer->DrawFromVertexArray(flameVertexes, 3, m_position, m_orientationAngleInDegree, m_visualDisc2.radius);
	}
}

void Ship::drawShipWithScale(float scale) const {
	// Draw Ship
	g_theRenderer->DrawFromVertexArray(m_shipVertexPoint, 5, m_position, m_orientationAngleInDegree, scale);

	// Draw flames
	if(m_thrustButtonPressed == true) {

		float flamesEndX = MathUtil::GetRandomFloatInRange(-5, -3) / 8.f;		// Get Flame's Random End's x-axis
		Vector2 flameEndVertex = Vector2(flamesEndX, 0.f);
		Vector2 flameVertexes[3] = {m_shipVertexPoint[2], m_shipVertexPoint[3], flameEndVertex};

		g_theRenderer->DrawFromVertexArray(flameVertexes, 3, m_position, m_orientationAngleInDegree, scale);
	}
}

void Ship::rotateShipByDegreesPerSeconds(float rotation, float deltaTime) {
	m_orientationAngleInDegree += (rotation * deltaTime);
}

void Ship::thrustShipAheadByUnitsPerSeconds(float acceleration, float deltaTime) {
	// Calculating thrust vector
	float distanceTravelledInThisFrame = acceleration * deltaTime;
	Vector2 direction = Vector2( MathUtil::CosDegree(m_orientationAngleInDegree), MathUtil::SinDegree(m_orientationAngleInDegree));
	Vector2 thrust = distanceTravelledInThisFrame * direction;

	// Add thrust to velocity to modify it
	m_velocity += thrust;
}

Vector2 Ship::getBulletSpawnPoint() {
	// Get nose's direction from center of Ship
	Vector2 noseRelativeDirection = Vector2( MathUtil::CosDegree(m_orientationAngleInDegree), MathUtil::SinDegree(m_orientationAngleInDegree));

	// How far the nose is from center of Ship
	float radiusToNose = m_visualDisc2.radius/2.f;

	// Calculate the exact position of nose
	Vector2 shipsNose = (radiusToNose * noseRelativeDirection) + m_position;
	
	return shipsNose;
}

