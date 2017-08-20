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
#include "graphics\camera\FPSCamera.h"
#include "utils\Logger.h"
#include "graphics\Model.h"
#include "terrain\Terrain.h"
#include "Scene3D.h"
#include "platform\OpenGL\Framebuffer.h"
#include "graphics\MeshFactory.h"



int main() {
	arcane::graphics::FPSCamera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
	arcane::graphics::Window window("Arcane Engine", 1366, 768);
	arcane::Scene3D scene(&camera, &window);
	arcane::opengl::Framebuffer framebuffer(window.getWidth(), window.getHeight());
	arcane::graphics::Shader framebufferShader("src/shaders/framebufferColourBuffer.vert", "src/shaders/framebufferColourBuffer.frag");
	arcane::graphics::MeshFactory meshFactory;
	arcane::graphics::Mesh *colourBufferMesh = meshFactory.CreateScreenQuad(framebuffer.getColourBufferTexture());

	arcane::Timer fpsTimer;
	int frames = 0;

	arcane::Time deltaTime;
	bool firstMove = true;
	GLfloat lastX = window.getMouseX();
	GLfloat lastY = window.getMouseY();
	while (!window.closed()) {
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		window.clear();
		deltaTime.update();

		// Check to see if the mouse hasn't been moved yet
		if (firstMove && (lastX != window.getMouseX() || lastY != window.getMouseY())) {
			lastX = window.getMouseX();
			lastY = window.getMouseY();
			firstMove = false;
		}

		// Camera Update
		camera.processMouseMovement(window.getMouseX() - lastX, lastY - window.getMouseY(), true);
		lastX = window.getMouseX();
		lastY = window.getMouseY();
		if (window.isKeyPressed(GLFW_KEY_W))
			camera.processKeyboard(arcane::graphics::FORWARD, deltaTime.getDeltaTime());
		if (window.isKeyPressed(GLFW_KEY_S))
			camera.processKeyboard(arcane::graphics::BACKWARD, deltaTime.getDeltaTime());
		if (window.isKeyPressed(GLFW_KEY_A))
			camera.processKeyboard(arcane::graphics::LEFT, deltaTime.getDeltaTime());
		if (window.isKeyPressed(GLFW_KEY_D))
			camera.processKeyboard(arcane::graphics::RIGHT, deltaTime.getDeltaTime());
		if (window.isKeyPressed(GLFW_KEY_SPACE))
			camera.processKeyboard(arcane::graphics::UPWARDS, deltaTime.getDeltaTime());
		if (window.isKeyPressed(GLFW_KEY_LEFT_CONTROL))
			camera.processKeyboard(arcane::graphics::DOWNWARDS, deltaTime.getDeltaTime());
		camera.processMouseScroll(window.getScrollY() * 6);
		window.resetScroll();
		 
		// Draw the scene to our custom framebuffer
		framebuffer.bind();
		window.clear();
		scene.onUpdate(deltaTime.getDeltaTime());
		scene.onRender();

		// Draw to the default scene buffer
		framebuffer.unbind();
		glClear(GL_COLOR_BUFFER_BIT);
		framebufferShader.enable();
		colourBufferMesh->Draw(framebufferShader);
		framebufferShader.disable();
		

		window.update();
		if (fpsTimer.elapsed() >= 1) {
			std::cout << "FPS: " << frames << std::endl;
			frames = 0;
			fpsTimer.reset();
		}
		else {
			frames++;
		}
	}
	return 0;
}