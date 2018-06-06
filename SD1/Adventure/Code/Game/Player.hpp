#pragma once
#include "Game/Actor.hpp"

class Player : public Actor
{
public:
	 Player( Vector2 position, float orientation, std::string definitionName );

	 void Update( float deltaSeconds );
	 void Render();

	 void PerformBehaviourActions( float deltaSeconds );
	 void ResetHealth();

private:
	void ShootAProjectile();
};