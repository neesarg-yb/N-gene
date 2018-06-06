#pragma once

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/GameCommon.hpp"
#include "Game/theGame.hpp"

extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern theGame* g_theGame;
extern AudioSystem* g_theAudioSystem;