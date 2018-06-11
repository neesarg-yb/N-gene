#pragma once
#include "Game/World/Block.hpp"
#include "Game/World/TowerDefinition.hpp"

class Tower : public GameObject
{
public:
	 Tower( Vector3 position, std::string towerDefinitionName );	// position: of bottom center
	~Tower();

public:
	std::vector< Block* >	 m_allBlocks;
	TowerDefinition const	&m_definition;

public:
	void Update( float deltaSeconds );
};