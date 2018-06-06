#pragma once

#include "Entity.hpp"
#include "ActorDefinition.hpp"

Entity::Entity( Vector2 position, float orientationInDegrees, EntityDefinition& entityDefinition )
	: m_position( position )
	, m_orientationAngleInDegree( orientationInDegrees )
	, m_definition( &entityDefinition )
	, m_tags( entityDefinition.m_startTags )
{
	 m_animationSet = new SpriteAnimSet( m_definition->m_animSetDef );
}


void Entity::Update( float deltaSeconds )
{
	// If health is zero, it should die
	if( m_currentHealth <= 0.f )
		m_readyToDie = true;

	// Move to new position according the velocity
	m_position += ( m_velocity * deltaSeconds );

	// Switch to the right animation
	if( m_velocity == Vector2::ZERO )
	{
		m_animationSet->SetCurrentAnim("Idle");
	}
	else
	{
		// Get all the dotProducts
		float dotProductNorth	= Vector2::DotProduct( m_velocity, Vector2::TOP_DIR );
		float dotProductSouth	= Vector2::DotProduct( m_velocity, Vector2::BOTTOM_DIR );
		float dotProductEast	= Vector2::DotProduct( m_velocity, Vector2::RIGHT_DIR );
		float dotProductWest	= Vector2::DotProduct( m_velocity, Vector2::LEFT_DIR );

		// Set animation according to highest dotProduct
		float maxDotProduct		= dotProductNorth;
		if( m_animationSet->HasAnimationByName("MoveNorth") )
			m_animationSet->SetCurrentAnim("MoveNorth");

		if( dotProductSouth > maxDotProduct )
		{
			maxDotProduct = dotProductSouth;
			if( m_animationSet->HasAnimationByName("MoveSouth") )
				m_animationSet->SetCurrentAnim("MoveSouth");
		}
		if( dotProductWest > maxDotProduct )
		{
			maxDotProduct = dotProductWest;
			if( m_animationSet->HasAnimationByName("MoveWest") )
				m_animationSet->SetCurrentAnim("MoveWest");
		}
		if( dotProductEast > maxDotProduct )
		{
			maxDotProduct = dotProductEast;
			if( m_animationSet->HasAnimationByName("MoveEast") )
				m_animationSet->SetCurrentAnim("MoveEast");
		}
	}

	m_animationSet->Update( deltaSeconds );
}

void Entity::Render()
{
	Vector2 boundsMin = m_definition->m_visualLocalBounds.mins;
	Vector2 boundsMax = m_definition->m_visualLocalBounds.maxs;
	boundsMin += m_position;
	boundsMax += m_position;
	AABB2 boundsForAnimation = AABB2( boundsMin, boundsMax );

	AABB2 textCoords = m_animationSet->GetCurrentUVs();
	g_theRenderer->DrawTexturedAABB( boundsForAnimation, m_animationSet->GetCurrentTexture(), textCoords.mins, textCoords.maxs, RGBA_WHITE_COLOR);

	if( m_showDebugInfo == true )
		DrawDebugInformations();
}


void Entity::LoadDefinitions()
{

}

float Entity::GetSpeed() const 
{
	return m_velocity.GetLength();
}

void Entity::SetSpeed(float speed) 
{
	m_velocity.NormalizeAndGetLength();
	m_velocity *= ClampFloat( speed, -m_definition->m_maxSpeed, +m_definition->m_maxSpeed );
}

float Entity::GetVelocityDirectionInDegree() const 
{
	return m_velocity.GetOrientationDegrees();
}

void Entity::SetVelocityDirectionInDegree(float speed , float degree) 
{
	float lengthOfVelocity = ClampFloat( speed, -m_definition->m_maxSpeed, +m_definition->m_maxSpeed );
	m_velocity = Vector2( (lengthOfVelocity * CosDegree(degree)) , (lengthOfVelocity * SinDegree(degree)) ); 
}

void Entity::DrawDebugInformations() const 
{
	const Rgba magentaColor = Rgba(255, 0, 255, 255);
	const Rgba cyanColor = Rgba(0, 255, 255, 255);
	const Rgba yellowColor = Rgba(255, 255, 0, 255);

	// Drawing VisualDisc
	// g_theRenderer->DrawDottedPolygon( m_position, m_definition->m_visualRadius, 24.f, 0.f, magentaColor);

	// Drawing CollosionDisc
	g_theRenderer->DrawDottedPolygon(m_position, m_definition->m_collisionRadius, 24.f, 0.f, cyanColor);

	// Drawing VelocityVector
	Vector2 endOfVelocityVector = m_position + m_velocity;
	g_theRenderer->DrawLine(m_position, endOfVelocityVector, yellowColor, yellowColor, 1.5f);
	
}

Disc2 Entity::GetCollisionDisc() const
{
	Disc2	collisionDisc = Disc2( m_position, m_definition->m_collisionRadius );
	return	collisionDisc;
}