#pragma once
#include "EngineCommon.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/LogSystem/LogSystem.hpp"
#include "Engine/Network/Network.hpp"

Clock *g_engineClock = nullptr;

void EngineStartup()
{
	// Master Clock Creation
	g_engineClock = new Clock();

	// LogSystem Startup
	if( g_logSystemEnabled )	
		LogSystem::GetInstance()->LoggerStartup();

	// Renderer Startup
	Renderer::RendererStartup();

	// Profiler Startup
	Profiler::Startup();

	// Network Startup
	if( g_networkSessionEnabled )
		Network::Startup();
}

void EngineShutdown()
{
	if( g_networkSessionEnabled )
		Network::Shutdown();
	
	Profiler::Shutdown();

	Renderer::RendererShutdown();
	Renderer::GLShutdown();

	if( g_logSystemEnabled )
		LogSystem::GetInstance()->LoggerShutdown();

	delete g_engineClock;
}

Clock const* GetMasterClock()
{
	return g_engineClock;
}

void TickMasterClock()
{
	g_engineClock->BeginFrame();
}
