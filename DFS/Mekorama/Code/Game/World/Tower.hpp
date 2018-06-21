#pragma once
#include "Engine/Math/IntVector3.hpp"
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
	void	Update( float deltaSeconds );
	void	SetFinishBlockAt( IntVector3 finishPos );
	uint	GetIndexOfBlockAt( IntVector3 blockPos );
};