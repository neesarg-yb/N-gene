#pragma once
#include <vector>
#include "Game/Entity.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/AIBehavior.hpp"
#include "Engine/Math/HeatMap.hpp"

class Projectile;

class Actor : public Entity
{
public:
			 Actor( Vector2 position, float orientationInDegrees, std::string entityDefName );
	virtual ~Actor();

	ActorDefinition*			m_actorDefinition	= nullptr;
	std::vector< AIBehavior* >	m_AIBehaviors;

	HeatMap*			m_currentHeatMap		= nullptr;
	AIBehavior*			m_currentAIBehavior		= nullptr;
	bool				m_wanderAndShootPlayer	= false;
	const float			projectilesRange		= 2.f;

	virtual void Update( float deltaSeconds );
	virtual void Render();

	virtual void	PerformBehaviourActions( float deltaSeconds );								// AI_Support OR InputControl code goes here. It will be called in Update().
	Projectile*		CreateAProjectile( float shootingRate );
	void			UpdateTheHeatMap( HeatMap* theHeatMap, const Map& currentAdventureMap );	// Note: If theHatMap == nullptr, it won't update

	float	m_timeElaspedSinceLastShooting		= 0.f;

private:
	float	m_timeElaspedSinceLastGoalChanged	= 0.f;
	float	m_goalPositionChangeAfterSeconds	= 5.f;
	Vector2 m_goalPosition						= Vector2::ZERO;
};