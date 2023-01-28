#pragma once

#ifdef ARC_PLATFORM_WINDOWS

#include <arcpch.h>
#include <Arcane/Core/Application.h>
#include <Arcane/Core/Base.h>
#include <Arcane/RenderdocManager.h>

extern Arcane::Application* Arcane::CreateApplication(int argc, char **argv);
bool g_ApplicationRunning = true;

int main(int argc, char **argv)
{
#ifdef ARC_RENDERDOC_DEBUG
	// Load in renderdoc api
	RENDERDOCMANAGER;
#endif

	while (g_ApplicationRunning)
	{
		Arcane::Application *app = Arcane::CreateApplication(argc, argv);
		ARC_ASSERT(app, "Application is null!");
		app->Run();
		delete app;
	}
}

#endif
