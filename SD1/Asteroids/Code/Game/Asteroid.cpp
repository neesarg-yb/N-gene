#include "Asteroid.hpp"

Asteroid::Asteroid() {

}

Asteroid::Asteroid(float xPosition, float yPosition, float radius)
{
	// Set all the member variables of the Entity class
	m_position = Vector2(xPosition, yPosition);

	// Get random velocity direction
	float velocityDirectionInDegree = (float) MathUtil::GetRandomNonNegativeIntLessThan(360);
	for ( bool goodAngleFound = false; goodAngleFound != true; velocityDirectionInDegree = (float) MathUtil::GetRandomNonNegativeIntLessThan(360) )
	{
		if( (int)velocityDirectionInDegree == 0 || (int)velocityDirectionInDegree == 90 || (int)velocityDirectionInDegree == 180 || (int)velocityDirectionInDegree == 270) {
			goodAngleFound = false;

		} else {
			goodAngleFound = true;
		}
	}

	SetVelocityDirectionInDegree( 150.f , velocityDirectionInDegree );
	SetSpeed( 150.f );
	m_orientationAngleInDegree = 0;
	m_rotationSpeedInDegreePerSeconds = MathUtil::GetRandomFloatInRange(50, 150);
	m_rotationSpeedMultiplier = MathUtil::GetRandomFloatAsPlusOrMinusOne();

	m_visualDisc2 = Disc2(m_position, radius);
	m_collosionDisc2 = Disc2(m_position, radius * 0.80f);

	// add data to vertexPoint[30]
	// For loop for every vertex
	for(float i=0; i<m_sides; i++) {
		float startAngle = ( i * (360.f/m_sides) );

		float randomnessMultiplier = MathUtil::GetRandomIntInRange(80, 100) / 100.f;											// Making the uneven boundary 100% to 80%
		m_vertexPoint[(int) i].x = randomnessMultiplier * MathUtil::CosDegree(startAngle + m_orientationAngleInDegree);			// For unit asteroid, it'll be scaled by "radius" later
		m_vertexPoint[(int) i].y = randomnessMultiplier * MathUtil::SinDegree(startAngle + m_orientationAngleInDegree);			// For unit asteroid, it'll be scaled by "radius" later
	}
}

Asteroid::~Asteroid()
{
}

void Asteroid::Update(float deltaTime) {
	CheckAndWrapAround();

	m_position += (deltaTime * m_velocity);
	m_orientationAngleInDegree += (m_rotationSpeedInDegreePerSeconds * m_rotationSpeedMultiplier * deltaTime);
	m_visualDisc2.center = m_position;
	m_collosionDisc2.center = m_position;
}

void Asteroid::drawAsteroid() {
	g_theRenderer->DrawFromVertexArray(m_vertexPoint, (int)m_sides, m_position, m_orientationAngleInDegree, m_visualDisc2.radius);
}