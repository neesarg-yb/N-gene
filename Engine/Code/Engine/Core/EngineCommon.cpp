#pragma once
#include "EngineCommon.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/LogSystem/LogSystem.hpp"

void EngineStartup()
{
	// LogSystem Startup
	LogSystem::GetInstance()->LoggerStartup( "log" );

	// Renderer Startup
	Renderer::RendererStartup();

	// Profiler Startup
	Profiler::Startup();
}

void EngineShutdown()
{
	Profiler::Shutdown();

	Renderer::RendererShutdown();
	Renderer::GLShutdown();

	LogSystem::GetInstance()->LoggerShutdown();
}

