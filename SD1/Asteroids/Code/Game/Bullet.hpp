#pragma once

#include "Polygon2.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Game/GameCommon.hpp"

class Bullet : public Entity						// Using position, velocity, and two Disc2 from Entity
{
public:
	bool m_isDead = false;

	Polygon2 m_body;
	float m_age = 0.f;								// Bullet's age; bullets will dieAfter 4 seconds
	const float m_dieAfterTime = 2.f;				// In seconds
	const float m_speed = 500.f;					// Bullet's speed in units/seconds

	Bullet();
	Bullet(const Vector2& position, const Vector2& unitDirection);
	~Bullet();

	void Update(float deltaTime);
	void drawBullet();

private:

};
