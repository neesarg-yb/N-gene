#pragma once
#include "Engine/Core/Blackboard.hpp"

#define UNUSED(x) (void)(x);

typedef tinyxml2::XMLElement XMLElement;
typedef tinyxml2::XMLAttribute XMLAttribute;

extern Blackboard* g_gameConfigBlackboard;	// declare here, and defined in .cpp