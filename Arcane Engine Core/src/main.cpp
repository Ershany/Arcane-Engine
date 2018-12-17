#include "pch.h"

#include <graphics/Shader.h>
#include <graphics/Window.h>
#include <graphics/camera/FPSCamera.h>
#include <graphics/mesh/Model.h>
#include <graphics/mesh/common/Quad.h>
#include <graphics/renderer/GLCache.h>
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
	arcane::FPSCamera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
	arcane::Window window("Arcane Engine", WINDOW_X_RESOLUTION, WINDOW_Y_RESOLUTION);
	arcane::Scene3D scene(&camera, &window);
	arcane::GLCache *glCache = arcane::GLCache::getInstance();
	arcane::PostProcessor postProcessor(scene.getRenderer());
	arcane::TextureLoader::initializeDefaultTextures();

	// Prepare the UI
	arcane::RuntimePane runtimePane(glm::vec2(256.0f, 90.0f));
	arcane::DebugPane debugPane(glm::vec2(256.0f, 115.0f));

	// Construct framebuffers
	bool shouldMultisample = MSAA_SAMPLE_AMOUNT > 1.0 ? true : false;
	arcane::Framebuffer framebuffer(window.getWidth(), window.getHeight());
	framebuffer.addTexture2DColorAttachment(shouldMultisample).addDepthStencilRBO(shouldMultisample).createFramebuffer();
	arcane::Framebuffer shadowmap(SHADOWMAP_RESOLUTION_X, SHADOWMAP_RESOLUTION_Y);
	shadowmap.addDepthAttachment(false).createFramebuffer();

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

		// Shadowmap Pass
#if DEBUG_ENABLED
		glFinish();
		timer.reset();
#endif
		glViewport(0, 0, shadowmap.getWidth(), shadowmap.getHeight());
		shadowmap.bind();
		shadowmap.clear();
		scene.shadowmapPass();
#if DEBUG_ENABLED
		glFinish();
		runtimePane.setShadowmapTimer((float)timer.elapsed());
#endif

		// Camera Update
		camera.processInput((float)deltaTime.getDeltaTime());

		// Draw the scene to our custom multisampled framebuffer
		glViewport(0, 0, framebuffer.getWidth(), framebuffer.getHeight());
		framebuffer.bind();
		framebuffer.clear();
		scene.onUpdate((float)deltaTime.getDeltaTime());
		scene.onRender(shadowmap.getDepthTexture());

		// Peform post processing
		postProcessor.postLightingPostProcess(&framebuffer);

		// Display panes
		runtimePane.render();
		debugPane.render();

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		window.update();
	}
	return 0;
}
