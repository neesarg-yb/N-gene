#pragma once
#include "Game/GameObject.hpp"
#include "Engine/Math/IntVector3.hpp"

class Block;
class Tower;
class HeatMap3D;

class Robot: public GameObject
{
public:
	 Robot( IntVector3 const &posInTower, Tower *parentTower );
	~Robot();

public:
	float const		 m_speed			= 4.f;				// Units(blocks) per seconds
	IntVector3		 m_targetPosition	= IntVector3::ZERO;
	IntVector3		 m_nextStepPosition	= IntVector3::ZERO;
	Tower			*m_parentTower		= nullptr;

private:
	bool			 m_stopPathFinding	= false;

public:
	void		Update( float deltaSeconds );
	void		ObjectSelected();

	void		SetParentTower( Tower &parent );
	void		SetPositionInTower( IntVector3 const &posInTower );
	IntVector3	GetPositionInTower() const;

	void		StopPathFinding();
	void		ResetPathFinding();
	void		SetTargetBlock( Block &targetBlock );
	void		MoveTowardsPosition( Vector3 const &destination, float deltaSeconds );
};