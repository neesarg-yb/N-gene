#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Game/World/BlockLocator.hpp"

struct RaycastResult_MC
{
public:
	RaycastResult_MC();
	RaycastResult_MC( Vector3 const &startPosition, Vector3 const &direction, float maxDistance, float impactFraction, BlockLocator const &impactBlock, Vector3 const &impactNormal );

public:
	Vector3			m_startPosition		= Vector3::ZERO;
	Vector3			m_direction			= Vector3::ZERO;
	float			m_maxDistance		= 0.f;
	float			m_impactFraction	= 0.f;
	BlockLocator	m_impactBlock		= BlockLocator::INVALID;
	Vector3			m_impactNormal		= Vector3::ZERO;

	// Can be calculated
	Vector3			m_endPosition		= Vector3::ZERO;
	Vector3			m_impactPosition	= Vector3::ZERO;
	float			m_impactDistance	= 0.f;

public:
	bool DidImpact() const;
};

inline bool RaycastResult_MC::DidImpact() const
{
	return m_impactFraction < 1.f;
}
