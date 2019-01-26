#pragma once
#include "CameraConstraint.hpp"

CameraConstraint::CameraConstraint( char const *name, CameraManager &manager, uint8_t priority )
	: m_name( name )
	, m_manager( manager )
	, m_priority( priority )
{

}

CameraConstraint::~CameraConstraint()
{

}

bool CameraConstraint::operator < ( CameraConstraint const& b ) const
{
	return (m_priority < b.m_priority);
}

bool CameraConstraint::operator > ( CameraConstraint const& b ) const
{
	return (m_priority > b.m_priority);
}
