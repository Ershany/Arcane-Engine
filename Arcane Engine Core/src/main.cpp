#include <iostream>
#include "graphics\Window.h"
#include "utils\Timer.h"
#include "graphics\Shader.h"
#include <SOIL.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils\Time.h"
#include "graphics\camera\Camera.h"
#include "utils\Logger.h"
#include "graphics\mesh\Model.h"
#include "terrain\Terrain.h"
#include "Scene3D.h"
#include "platform\OpenGL\Framebuffers\RenderTarget.h"
#include "graphics/mesh/common/Quad.h"
#include "graphics/renderer/GLCache.h"
#include "graphics/renderer/PostProcessor.h"
#include "ui/RuntimePane.h"
#include "ui/DebugPane.h"



int main() {
	// Prepare the engine
	arcane::graphics::Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
	arcane::graphics::Window window("Arcane Engine", WINDOW_X_RESOLUTION, WINDOW_Y_RESOLUTION);
	arcane::Scene3D scene(&camera, &window);
	arcane::graphics::GLCache *glCache = arcane::graphics::GLCache::getInstance();
	arcane::graphics::PostProcessor postProcessor(scene.getRenderer());
	arcane::utils::TextureLoader::initializeDefaultTextures();

	// Prepare the UI
	arcane::ui::RuntimePane runtimePane(glm::vec2(256.0f, 90.0f));
	arcane::ui::DebugPane debugPane(glm::vec2(256.0f, 100.0f));

	// Construct framebuffers
	bool shouldMultisample = MSAA_SAMPLE_AMOUNT > 1.0 ? true : false;
	arcane::opengl::RenderTarget framebuffer(window.getWidth(), window.getHeight());
	framebuffer.addColorAttachment(shouldMultisample).addDepthStencilRBO(shouldMultisample).createFramebuffer();
	// TODO: MAKE MULTISAMPLE OPTION WORK OR INVESTIGATE
	arcane::opengl::RenderTarget shadowmap(SHADOWMAP_RESOLUTION_X, SHADOWMAP_RESOLUTION_Y);
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
		runtimePane.setShadowmapTimer(timer.elapsed());
#endif

		// Camera Update
		camera.processInput(deltaTime.getDeltaTime());

		// Draw the scene to our custom multisampled framebuffer
		glViewport(0, 0, framebuffer.getWidth(), framebuffer.getHeight());
		framebuffer.bind();
		framebuffer.clear();
		scene.onUpdate(deltaTime.getDeltaTime());
		scene.onRender(shadowmap.getDepthTexture());

		// Peform post processing
		postProcessor.postLightingPostProcess(&framebuffer);

		// Display panes
		runtimePane.render();
		debugPane.render();

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		window.resetScroll();
		window.update();
	}
	return 0;
}