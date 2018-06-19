#pragma once
#include "Game/GameObject.hpp"
#include "Game/World/BlockDefinition.hpp"

class Block : public GameObject
{
public:
	 Block( Vector3 const &position, std::string blockDefinitionName );
	~Block();

public:
	BlockDefinition const &m_definition;

public:
	void Update( float deltaSeconds );
	void ObjectSelected();

private:
	AABB2 GetUVBoundsFromCoord( IntVector2 spriteSheetDimension, IntVector2 uvCoord );		// uvCoord start from (0, 0)
};