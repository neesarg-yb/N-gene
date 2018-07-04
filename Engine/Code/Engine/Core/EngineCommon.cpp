#pragma once
#include "EngineCommon.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Renderer/Renderer.hpp"

void EngineStartup()
{
	Renderer::RendererStartup();

	// Create the Profiler Instance
	Profiler::Startup();
}

void EngineShutdown()
{
	Profiler::Shutdown();

	Renderer::RendererShutdown();
	Renderer::GLShutdown();
}

