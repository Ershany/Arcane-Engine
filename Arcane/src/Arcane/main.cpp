#include "arcpch.h"

#include <Arcane/Core/Application.h>

int main() {

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
	Arcane::Application::GetInstance().Run();

	return EXIT_SUCCESS;
}
