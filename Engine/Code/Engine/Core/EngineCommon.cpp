#pragma once
#include "EngineCommon.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/LogSystem/LogSystem.hpp"
#include "Engine/Network/Network.hpp"

void EngineStartup()
{
	// LogSystem Startup
	LogSystem::GetInstance()->LoggerStartup();

	// Renderer Startup
	Renderer::RendererStartup();

	// Profiler Startup
	Profiler::Startup();

	// Network Startup
	Network::Startup();

	GetAddressExample();
}

void EngineShutdown()
{
	Network::Shutdown();
	Profiler::Shutdown();

	Renderer::RendererShutdown();
	Renderer::GLShutdown();

	LogSystem::GetInstance()->LoggerShutdown();
}

