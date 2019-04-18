#include "pch.h"

#include <graphics/Window.h>
#include <graphics/renderer/MasterRenderer.h>
#include <scene/Scene3D.h>
#include <ui/DebugPane.h>
#include <ui/RuntimePane.h>
#include <ui/NavmeshPane.h>
#include <utils/Time.h>

int main() {
	// Prepare the engine
	arcane::Window window("Arcane Engine", WINDOW_X_RESOLUTION, WINDOW_Y_RESOLUTION);
	arcane::TextureLoader::initializeDefaultTextures();
	arcane::Scene3D scene(&window);
	arcane::MasterRenderer renderer(&scene);

	// Prepare the UI
	arcane::RuntimePane runtimePane(glm::vec2(256.0f, 90.0f));
	arcane::DebugPane debugPane(glm::vec2(256.0f, 115.0f));
	arcane::NavmeshPane navmeshPane(glm::vec2(256.0f, 200.0f));

	// Initialize the renderer
	renderer.init();

	arcane::Time deltaTime;
	while (!window.closed()) {
		deltaTime.update();

#if DEBUG_ENABLED
		if (debugPane.getWireframeMode())
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

		window.clear();
		ImGui_ImplGlfwGL3_NewFrame();

		scene.onUpdate((float)deltaTime.getDeltaTime());
		renderer.render();
		scene.onRender();

		// Display panes
		runtimePane.render();
		debugPane.render();
		navmeshPane.render();

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		window.update();
	}
	return 0;
}
