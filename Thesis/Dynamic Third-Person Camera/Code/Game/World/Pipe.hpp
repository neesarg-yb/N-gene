#pragma once
#include "Engine/Math/IntVector3.hpp"
#include "Game/GameObject.hpp"

enum ePipeForwardDirection
{
	PIPE_DIR_WORLD_FRONT = 0,
	PIPE_DIR_WORLD_BACK,
	PIPE_DIR_WORLD_LEFT,
	PIPE_DIR_WORLD_RIGHT,
	PIPE_DIR_WORLD_UP,
	PIPE_DIR_WORLD_DOWN,
	NUM_PIPE_DIRECTIONS
};

struct PipeSpawnData
{
	IntVector3				positionInTower;
	ePipeForwardDirection	forwardDirection;
	float					length;

	PipeSpawnData( IntVector3 const &posInTower, ePipeForwardDirection forwardDir, float length )
	{
		this->positionInTower	= posInTower;
		this->forwardDirection	= forwardDir;
		this->length			= length;
	}
};

class Tower;

class Pipe: public GameObject
{
public:
	 Pipe( IntVector3 const &startPos, ePipeForwardDirection pipeForward, float length, Tower const &parentTower );
	~Pipe();

private:
	Tower const &m_parentTower;

public:
	IntVector3	const m_startPosition;
	Vector3		const m_forwardDirection;
	float		const m_length;

public:
	void Update( float deltaSeconds );
	void ObjectSelected();

public:
	static	Vector3 GetDirectionAsVector3	( ePipeForwardDirection pipeDir );
			Vector3 GetRotationFromForward	( ePipeForwardDirection forward );
};