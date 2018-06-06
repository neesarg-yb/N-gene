#pragma  once
#include <vector>
#include <string>
#include "Engine/Math/HeatMap.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Core/Image.hpp"
#include "Game/Block.hpp"
#include "Game/BlockDefinition.hpp"
#include "Game/GameCommon.hpp"

enum DirectionEnum
{
	LEFT_DIR = 0,
	RIGHT_DIR,
	TOP_DIR,
	BOTTOM_DIR,
	NUM_DIRECTIONS
};

class Map
{
public:
	 Map( std::string depth_image, int map_height, BlockDefinition* default_block_definition );
	~Map();

	void Update( float deltaSeconds );
	void Render() const;

	Vector3 GiveWorldPositionToRenderOnTopOfTheBlockAt( int x, int z ) const;

public:
	IntVector3				m_dimensions;
	std::vector< Block* >	m_map_blocks;

	int		IndexOfBlockAt( int x, int y, int z ) const;
	HeatMap GenerateHeatMapForPositionAndJumpHeight( int x, int z, int jumpHeight ) const;

private:
	IntVector2 GetNeighbourTileCoordinate( int x, int z, DirectionEnum direction ) const;		// Returns IntVector( -1, -1 ) if Neighbor doesn't exist
};