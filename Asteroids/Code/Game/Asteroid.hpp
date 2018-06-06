#pragma once

#include "Entity.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Game/GameCommon.hpp"


class Asteroid : public Entity
{
public:
	void drawAsteroid();
	void Update(float deltaTime);

	Asteroid(float xPosition, float yPosition, float radius);
	Asteroid();
	~Asteroid();

private:
	const float m_sides = 20;			// WARNING: Change vertexPoints[], too!
	Vector2 m_vertexPoint[20];			//                   <------~)

};