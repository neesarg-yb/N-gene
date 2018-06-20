#pragma once
#include "Tower.hpp"

Tower::Tower( Vector3 position, std::string towerDefinitionName )
	: m_definition( *TowerDefinition::s_definitions[ towerDefinitionName ] )
{
	// Set tower's transform
	m_transform.SetPosition( position );

	// Create Blocks
	uint	blocksPerSlice	= m_definition.m_xzDimension.x * m_definition.m_xzDimension.y;
	uint	numSlices		= (uint) ( m_definition.m_blocksDefinitionList.size() / blocksPerSlice );
	Vector3 halfDimentions	= Vector3( (float)m_definition.m_xzDimension.x, (float)numSlices, (float)m_definition.m_xzDimension.y ) * 0.5f;

	// for each slices/layers
	for( uint sliceIdx = 0U; sliceIdx < numSlices; sliceIdx++ )
	{
		// columns
		for( uint colIdx = 0U; colIdx < (uint)m_definition.m_xzDimension.y; colIdx++ )
		{
			// rows
			for( uint rowIdx = 0U; rowIdx < (uint)m_definition.m_xzDimension.x; rowIdx++ )
			{
				Vector3 localPosition		 = Vector3( (float)rowIdx, (float)sliceIdx, (float)colIdx ) - halfDimentions;

				Transform m_blockTransform	 = Transform( localPosition, Vector3::ZERO, Vector3::ONE_ALL );
				m_blockTransform.SetParentAs( &m_transform );

				static uint nextBlkIdx		 = 0;
				Block*		thiBlock		 = new Block( m_blockTransform.GetWorldPosition(), m_definition.m_blocksDefinitionList[ nextBlkIdx ] );

				m_allBlocks.push_back( thiBlock );

				nextBlkIdx++;
			}
		}
	}
}

Tower::~Tower()
{
	for( uint i = 0; i < m_allBlocks.size(); i++ )
		delete m_allBlocks[i];

	m_allBlocks.clear();
}

void Tower::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

