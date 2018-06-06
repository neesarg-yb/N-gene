#pragma once
#include <string>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XMLUtilities.hpp"

class SpriteAnimSetDefinition
{
	friend class SpriteAnimSet;

public:
	 SpriteAnimSetDefinition( const XMLElement& animSetElement, Renderer& renderer );
	~SpriteAnimSetDefinition();

protected:
	std::map< std::string, SpriteAnimDefinition* >	m_namedAnimDefs;
	std::string										m_defaultAnimName = "Idle";
};
