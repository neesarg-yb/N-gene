#include "Entity.hpp"
#include "Game/GameCommon.hpp"

Entity::Entity() {
	m_markForDelete = false;
	m_position = Vector2( 0.f, 0.f );
	m_velocity = Vector2( 0.f, 0.f );
	m_orientationAngleInDegree = 00.f;
	m_rotationSpeedInDegreePerSeconds = 00.f;
	m_rotationSpeedMultiplier = 1.f;
	m_health = 3.0f;

	m_visualDisc2 = Disc2(m_position, 0.f);
	m_collosionDisc2 = Disc2(m_position, 0.f);
}

Entity::~Entity() {

}

void Entity::Render() {

}

void Entity::Update( float deltaSeconds ) {
	deltaSeconds = deltaSeconds;			// To ignore the warning..
}

Vector2 Entity::GetPosition() const {
	return m_position;
}

void Entity::SetPosition( const Vector2& newPosition ) {
	m_position = newPosition;
	m_visualDisc2.center = m_position;
	m_collosionDisc2.center = m_position;
}

float Entity::GetSpeed() const {
	float speed = m_velocity.GetLength();
	
	return speed;
}

void Entity::SetSpeed(float speed) {
	m_velocity.NormalizeAndGetLength();
	m_velocity *= speed;
}

float Entity::GetVelocityDirectionInDegree() const {
	float direction = m_velocity.GetOrientationDegrees();

	return direction;
}

void Entity::SetVelocityDirectionInDegree(float speed , float degree) {
	float lengthOfVelocity = speed;

	m_velocity = Vector2( (lengthOfVelocity * MathUtil::CosDegree(degree)) , (lengthOfVelocity * MathUtil::SinDegree(degree)) ); 
}

void Entity::CheckAndWrapAround() {
	float topAndRightEdge = 1000.f + m_visualDisc2.radius;
	float bottomAndLeftEdge = 0.f - m_visualDisc2.radius;

	if( m_position.x > (topAndRightEdge) ) {								// case: at right edge  | i.e. (x + Disc2.radius) > 1000
		m_position.x = 0.f - m_visualDisc2.radius;
	} 
	
	if( m_position.x < (bottomAndLeftEdge) ) {								// case: at left edge   | i.e. (x + Disc2.radius) < 0
		m_position.x = 1000.f + m_visualDisc2.radius;
	} 
	
	if( m_position.y > (topAndRightEdge) ) {								// case: at top edge    | i.e. (y + Disc2.radius) > 1000
		m_position.y = 0.f - m_visualDisc2.radius;
	} 
	
	if( m_position.y < (bottomAndLeftEdge) ) {								// case: at bottom edge | i.e. (y + Disc2.radius) < 0
		m_position.y = 1000.f + m_visualDisc2.radius;
	}
}


void Entity::drawDebugInformations() {
	Rgba magentaColor = Rgba(255, 0, 255, 255);
	Rgba cyanColor = Rgba(0, 255, 255, 255);
	Rgba yellowColor = Rgba(255, 255, 0, 255);

	// Drawing VisualDisc
	g_theRenderer->DrawDottedPolygon(m_visualDisc2.center, m_visualDisc2.radius, 24.f, 0.f, magentaColor);

	// Drawing CollosionDisc
	g_theRenderer->DrawDottedPolygon(m_collosionDisc2.center, m_collosionDisc2.radius, 24.f, 0.f, cyanColor);

	// Drawing VelocityVector
	Vector2 endOfVelocityVector = m_position + m_velocity;
	g_theRenderer->DrawLine(m_position, endOfVelocityVector, yellowColor, yellowColor, 1.5f);
}

void Entity::ReduceAHealth() {
	m_health -= 1;
	if( m_health <= 0)
	{
		m_markForDelete = true;
	}
}