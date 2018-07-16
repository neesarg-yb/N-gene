#pragma once
#include "Engine/Math/IntVector3.hpp"
#include "Game/World/Block.hpp"
#include "Game/World/TowerDefinition.hpp"

class HeatMap3D;

class Tower : public GameObject
{
public:
	 Tower( Vector3 position, std::string towerDefinitionName );	// position: of center
	~Tower();

public:
	std::vector< Block* >	 m_allBlocks;
	std::vector< Pipe* >	 m_allPipes;
	TowerDefinition const	&m_definition;
	IntVector3				 m_dimensionXYZ;

public:
	void	Update( float deltaSeconds );
	void	SetFinishBlockAt( IntVector3 const &finishPos );
	uint	GetIndexOfBlockAt( IntVector3 const &blockPos ) const;
	Vector3	GetWorldLocationOfBlockAt( IntVector3 const &blockPos ) const;

	Block*	GetBlockAt( IntVector3 const &blockPos );
	Block*	GetBlockOnTopOfMe( Block &baseBlock );
	Pipe*	GetAnchorPipeForBlock( Block const &block );

	std::vector< IntVector3 > GetNeighbourBlocksPos( IntVector3 const &myPosition );

public:
	HeatMap3D*	GetNewHeatMapForTargetPosition( IntVector3 targetPos ) const;
	bool		HasSolidBlockBeneath( IntVector3 const &myPosition ) const;
	bool		HasStairsBlockBeneath( IntVector3 const &myPosition ) const;
	bool		IsPositionOutsideTowersBounds( IntVector3 const &myPosition ) const;
};