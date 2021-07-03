#include "arcpch.h"

#include <graphics/Window.h>
#include <utils/Logger.h>

int main() {
	// Pre-Engine Intialization
	Arcane::Logger::GetEngineLogger();
	ARC_LOG_INFO("Initialized Logger");

	// Temp: Should be handled by whatever service that wants to run Arcane
	Arcane::ApplicationSpecification specification;
	specification.Name = "Arcane Engine";
	specification.WindowWidth = WINDOW_X_RESOLUTION;
	specification.WindowHeight = WINDOW_Y_RESOLUTION;
	specification.RenderResolutionWidth = specification.WindowWidth * SUPERSAMPLING_FACTOR;
	specification.RenderResolutionHeight = specification.WindowHeight * SUPERSAMPLING_FACTOR;
	specification.VSync = V_SYNC;
	specification.EnableImGui = true;
	new Arcane::Application(specification);

	//Arcane::Application::GetInstance().pushOverlay();
	Arcane::Application::GetInstance().run();

	return EXIT_SUCCESS;
}
