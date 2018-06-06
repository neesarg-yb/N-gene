#pragma once

#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\MathUtil.hpp"
#include "Engine\Math\Disc2.hpp"
#include "Game\Entity.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Core\Rgba.hpp"

class Ship : public Entity
{
public:
	bool m_thrustButtonPressed;													// Signals the ship to move ahead

	Ship();
	~Ship();

	void drawShip() const;														// Draws the ship
	void drawShipWithScale(float scale) const;
	void Update(float deltaTime);												// Local Update function
	void rotateShipByDegreesPerSeconds(float rotation, float deltaTime);		// To rotate the ship
	void thrustShipAheadByUnitsPerSeconds(float acceleration, float deltaTime);	// Accelerate
	Vector2 getBulletSpawnPoint();												// Returns the nose of the ship, from where bullet will be fired

private:

	// Ship's vertex in a 8x8 drawing
	Vector2 m_shipVertexPoint[5] = {	Vector2(   4.f/8.f,   0.f/8.f ),
										Vector2(  -4.f/8.f,  -3.f/8.f ),
										Vector2( -2.5f/8.f, -1.5f/8.f ),		// Flame's base vertex
										Vector2( -2.5f/8.f,  1.5f/8.f ),		// Flame's base vertex
										Vector2(  -4.f/8.f,   3.f/8.f )   };
};
