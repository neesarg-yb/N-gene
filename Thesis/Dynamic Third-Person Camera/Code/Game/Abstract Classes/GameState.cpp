#pragma once
#include "GameState.hpp"

GameState::GameState( std::string const &name, Clock const *parentClock /* = nullptr */ )
	: m_name( name )
{
	if( parentClock == nullptr )
		parentClock = GetMasterClock();

	m_clock = new Clock( parentClock );
}

GameState::~GameState()
{
	delete m_clock;
	m_clock = nullptr;
}
