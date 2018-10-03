#pragma once
#include "CameraConstrain.hpp"

CameraConstrain::CameraConstrain( char const *name, CameraManager &manager )
	: m_name( name )
	, m_manager( manager )
{

}

CameraConstrain::~CameraConstrain()
{

}
