#pragma once
#include <vector>
#include <string>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Tags.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/AABB2.hpp"

int					ParseXmlAttribute( const XMLElement& element, const char* attributeName, int defaultValue );
char				ParseXmlAttribute( const XMLElement& element, const char* attributeName, char defaultValue );
bool				ParseXmlAttribute( const XMLElement& element, const char* attributeName, bool defaultValue );
float				ParseXmlAttribute( const XMLElement& element, const char* attributeName, float defaultValue );
Rgba				ParseXmlAttribute( const XMLElement& element, const char* attributeName, const Rgba& defaultValue );
Vector2				ParseXmlAttribute( const XMLElement& element, const char* attributeName, const Vector2& defaultValue );
Vector3				ParseXmlAttribute( const XMLElement& element, const char* attributeName, const Vector3& defaultValue );
IntRange			ParseXmlAttribute( const XMLElement& element, const char* attributeName, const IntRange& defaultValue );
FloatRange			ParseXmlAttribute( const XMLElement& element, const char* attributeName, const FloatRange& defaultValue );
IntVector2			ParseXmlAttribute( const XMLElement& element, const char* attributeName, const IntVector2& defaultValue );
char const *		ParseXmlAttribute( const XMLElement& element, char const* attributeName, char const * defaultValue );
std::string			ParseXmlAttribute( const XMLElement& element, const char* attributeName, const std::string& defaultValue );
std::vector<int>	ParseXmlAttribute( const XMLElement& element, const char* attributeName, const std::vector<int>& defaultValue );
AABB2				ParseXmlAttribute( const XMLElement& element, const char* attributeName, const AABB2& defaultValue );
Tags				ParseXmlAttribute( const XMLElement& element, const char* attributeName, const Tags& defaultValue );