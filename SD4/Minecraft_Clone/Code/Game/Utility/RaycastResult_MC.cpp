#pragma once
#include "RaycastResult_MC.hpp"

RaycastResult_MC::RaycastResult_MC()
{

}

RaycastResult_MC::RaycastResult_MC( Vector3 const &startPosition, Vector3 const &direction, float maxDistance, float impactFraction, BlockLocator const &impactBlock, Vector3 const &impactNormal )
	: m_startPosition( startPosition )
	, m_direction( direction )
	, m_maxDistance( maxDistance )
	, m_impactFraction( impactFraction )
	, m_impactBlock( impactBlock )
	, m_impactNormal( impactNormal )
{
	m_impactDistance = m_maxDistance * m_impactFraction;
	m_endPosition	 = m_startPosition + (m_direction * m_maxDistance);
	m_impactPosition = m_startPosition + (m_direction * m_impactDistance);
}
