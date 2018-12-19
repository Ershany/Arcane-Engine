#include "pch.h"

#include <graphics/Shader.h>
#include <graphics/Window.h>
#include <graphics/camera/FPSCamera.h>
#include <graphics/mesh/Model.h>
#include <graphics/mesh/common/Quad.h>
#include <graphics/renderer/GLCache.h>
#include <graphics/renderer/MasterRenderer.h>
#include <graphics/renderer/PostProcessor.h>
#include <input/InputManager.h>
#include <platform/OpenGL/Framebuffers/Framebuffer.h>
#include <scene/Scene3D.h>
#include <terrain/Terrain.h>
#include <ui/DebugPane.h>
#include <ui/RuntimePane.h>
#include <utils/Time.h>
#include <utils/Timer.h>

int main() {
	// Prepare the engine
	arcane::Window window("Arcane Engine", WINDOW_X_RESOLUTION, WINDOW_Y_RESOLUTION);
	arcane::Scene3D scene(&window);
	arcane::MasterRenderer renderer(&scene);
	arcane::TextureLoader::initializeDefaultTextures();

	// Prepare the UI
	arcane::RuntimePane runtimePane(glm::vec2(256.0f, 90.0f));
	arcane::DebugPane debugPane(glm::vec2(256.0f, 115.0f));

#if DEBUG_ENABLED
	arcane::Timer timer;
#endif

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

		// Display panes
		runtimePane.render();
		debugPane.render();

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		window.update();
	}
	return 0;
}
