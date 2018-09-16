#pragma once
#include "Game/GameObject.hpp"
#include "Game/World/BlockDefinition.hpp"
#include "Engine/Math/IntVector3.hpp"

class Tower;

class Block : public GameObject
{
public:
	 Block( IntVector3 const &positionInTower, std::string blockDefinitionName, Tower const &parentTower );
	~Block();

public:
	BlockDefinition *m_definition	= nullptr;

private:
	Tower const		&m_parentTower;
	IntVector3		 m_posInTower	= IntVector3::ZERO;

public:
	void Update( float deltaSeconds );
	void ObjectSelected();
	void HighlightForSeconds( float seconds );
	void ChangeBlockTypeTo( std::string definitionName );
	void SetPositionInTower( IntVector3 const &posInTower );

	IntVector3	GetMyPositionInTower() const;
	inline bool IsDraggable() const { return m_definition->m_isDraggable; }

private:
	void		UpdateLocalTransformPosition();
	Renderable* CreateNewRenderable();		// Creates new Renderable according to m_definition
	AABB2		GetUVBoundsFromCoord( IntVector2 spriteSheetDimension, IntVector2 uvCoord );		// uvCoord start from (0, 0)
};