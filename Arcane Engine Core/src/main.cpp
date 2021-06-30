#include "pch.h"

#include <graphics/Window.h>
#include <graphics/renderer/MasterRenderer.h>
#include <scene/Scene3D.h>
#include <ui/DebugPane.h>
#include <ui/RuntimePane.h>
#include <ui/WaterPane.h>
#include <utils/Time.h>

int main() {
	// Prepare the engine
	Arcane::Window window("Arcane Engine", WINDOW_X_RESOLUTION, WINDOW_Y_RESOLUTION);
	Arcane::TextureLoader::initializeDefaultTextures();
	Arcane::Scene3D scene(&window);
	Arcane::MasterRenderer renderer(&scene);
	Arcane::InputManager manager;

	// Prepare the UI
	Arcane::RuntimePane runtimePane(glm::vec2(270.0f, 175.0f));
	Arcane::DebugPane debugPane(glm::vec2(270.0f, 400.0f));
	Arcane::WaterPane waterPane(glm::vec2(270.0f, 400.0f));

	// Initialize the renderer
	renderer.init();

	Arcane::Time deltaTime;
	while (!window.closed()) {
		deltaTime.update();

#if ARC_DEBUG
		if (debugPane.getWireframeMode())
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif // ARC_DEBUG

		Arcane::Window::bind();
		Arcane::Window::clear();
		ImGui_ImplGlfwGL3_NewFrame();

		scene.onUpdate((float)deltaTime.getDeltaTime());
		renderer.render();

		// Display panes
		if (!Arcane::Window::getHideUI())
		{
			Arcane::Window::bind();
			runtimePane.render();
			debugPane.render();
			waterPane.render();
		}
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		// Window and input updating
		window.update();
	}
	return 0;
}
