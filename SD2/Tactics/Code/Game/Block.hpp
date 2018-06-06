#pragma once
#include "Game/BlockDefinition.hpp"
#include "Game/GameCommon.hpp"

class Block
{
public:
	 Block( Vector3 position, BlockDefinition* definition = nullptr );
	~Block();

	void Update( float deltaSeconds );
	void Render() const;

public:
	BlockDefinition*	m_definition	= nullptr;					// = nullptr means it's an empty block
	Vector3				m_position		= Vector3::ZERO;			// World Position of Block
	
private:
	ShaderProgram*		m_customShader	= nullptr;
};