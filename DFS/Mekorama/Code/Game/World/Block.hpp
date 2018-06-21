#pragma once
#include "Game/GameObject.hpp"
#include "Game/World/BlockDefinition.hpp"

class Block : public GameObject
{
public:
	 Block( Vector3 const &position, std::string blockDefinitionName );
	~Block();

public:
	BlockDefinition *m_definition	= nullptr;

public:
	void Update( float deltaSeconds );
	void ObjectSelected();
	void ChangeBlockTypeTo( std::string definitionName );

private:
	Renderable* CreateNewRenderable();		// Creates new Renderable according to m_definition
	AABB2		GetUVBoundsFromCoord( IntVector2 spriteSheetDimension, IntVector2 uvCoord );		// uvCoord start from (0, 0)
};