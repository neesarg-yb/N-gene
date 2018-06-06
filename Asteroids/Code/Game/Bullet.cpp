#include "Bullet.hpp"


Bullet::Bullet()
{
}

Bullet::Bullet(const Vector2& position, const Vector2& unitDirection) {

	m_body.radius = 3.f;		// Polygon's radius
	m_body.sides = 4.f;		// Polygon's sides
	m_body.center = position;
	m_body.orientationAngle = unitDirection.GetOrientationDegrees();
	this->m_position = position;
	m_velocity = unitDirection * m_speed;
	m_visualDisc2 = Disc2(position, m_body.radius);
	m_collosionDisc2 = Disc2(position, m_body.radius-1);
}

Bullet::~Bullet()
{

}

void Bullet::Update(float deltaTime) {
	CheckAndWrapAround();

	m_position += (deltaTime * m_velocity);
	m_body.center = m_position;
	m_visualDisc2.center = m_position;
	m_collosionDisc2.center = m_position;

	m_age += deltaTime;
	if(m_age >= m_dieAfterTime) {
		m_isDead = true;
	}
}

void Bullet::drawBullet() {
	g_theRenderer->DrawPolygon(m_body.center, m_body.radius, m_body.sides, m_body.orientationAngle);
}