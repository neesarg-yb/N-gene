#pragma once
#include "Game/GameObject.hpp"
#include "Engine/Math/IntVector3.hpp"

class Block;
class Tower;
class HeatMap3D;

class Robot: public GameObject
{
public:
	 Robot( IntVector3 const &posInTower, Tower const *parentTower );
	~Robot();

public:
	IntVector3		 m_posInTower		= IntVector3::ZERO;
	Tower const		*m_parentTower		= nullptr;

public:
	void Update( float deltaSeconds );
	void ObjectSelected();

	void SetParentTower( Tower const &parent );
	void MoveAtBlock( Block &targetBlock );

private:
	void UpdateLocalTransform();
};