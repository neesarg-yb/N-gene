#pragma once
#include <string>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Polygon2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/SpriteAnimSet.hpp"
#include "Engine/Core/Tags.hpp"
#include "Game/GameCommon.hpp"
#include "Game/EntityDefinition.hpp"

class Entity {
public:
			 Entity( Vector2 position, float orientationInDegrees, EntityDefinition& entityDefinition );
	virtual ~Entity() { };

	void LoadDefinitions();

public:
	float				m_currentHealth		= 1.f;
	bool				m_readyToDie		= false;
	
	EntityDefinition*	m_definition		= nullptr;
	Tags				m_tags;

	Vector2				m_position;										// Position of the entity
	float				m_orientationAngleInDegree;						// Entity's facing direction
	Vector2				m_velocity			= Vector2( 0.f, 0.f );		// speed of the object

	bool				m_showDebugInfo		= false;
	SpriteAnimSet*		m_animationSet		= nullptr;

	float	GetSpeed() const;						// Get speed from velocity Vector2
	void	SetSpeed(float speed);					// Set speed using velocity Vector2
	float	GetVelocityDirectionInDegree() const;	// Get angle of velocity Vector2 in degree
	void	SetVelocityDirectionInDegree(float speed , float degree);		// Set angle of velocity Vector2 in degree
	Disc2	GetCollisionDisc() const;

	void	DrawDebugInformations() const;				// Draws info for Debug Mode

	virtual void	Update( float deltaSeconds );
	virtual void	Render();

private:
};