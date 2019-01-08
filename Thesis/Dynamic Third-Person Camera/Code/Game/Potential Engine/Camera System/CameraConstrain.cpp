#pragma once
#include "CameraConstrain.hpp"

CameraConstrain::CameraConstrain( char const *name, CameraManager &manager, uint8_t priority )
	: m_name( name )
	, m_manager( manager )
	, m_priority( priority )
{

}

CameraConstrain::~CameraConstrain()
{

}

bool CameraConstrain::operator < ( CameraConstrain const& b ) const
{
	return (m_priority < b.m_priority);
}

bool CameraConstrain::operator > ( CameraConstrain const& b ) const
{
	return (m_priority > b.m_priority);
}
