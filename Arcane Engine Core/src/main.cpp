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

arcane::graphics::FPSCamera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f); 
arcane::graphics::Window window("Arcane Engine", 1366, 768);
GLfloat lastX = window.getWidth();
GLfloat lastY = window.getHeight();
GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;

int main() {
	glEnable(GL_DEPTH_TEST);

	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
	};

	GLuint VBO, VAO, lightVAO;
	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0); // Unbind VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Light Cube
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0); //unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Light position
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	// Prepare the fps counter right before the first tick
	arcane::Timer timer;
	int frames = 0;

	// Temp Rotation Timer
	arcane::Timer count;
	arcane::Time time;

	arcane::graphics::Shader shader("src/shaders/basic.vert", "src/shaders/basic.frag");
	arcane::graphics::Shader lampShader("src/shaders/basic.vert", "src/shaders/lightCube.frag");

	lastX = window.getMouseX();
	lastY = window.getMouseY();
	while (!window.closed()) {
		window.clear();
		time.update();

		// Camera Processes
		camera.processMouseMovement(window.getMouseX() - lastX, lastY - window.getMouseY(), true);
		lastX = window.getMouseX();
		lastY = window.getMouseY();

		if (window.isKeyPressed(GLFW_KEY_W))
			camera.processKeyboard(arcane::graphics::FORWARD, time.delta);
		if (window.isKeyPressed(GLFW_KEY_S))
			camera.processKeyboard(arcane::graphics::BACKWARD, time.delta);
		if (window.isKeyPressed(GLFW_KEY_A))
			camera.processKeyboard(arcane::graphics::LEFT, time.delta);
		if (window.isKeyPressed(GLFW_KEY_D))
			camera.processKeyboard(arcane::graphics::RIGHT, time.delta);

		camera.processMouseScroll(window.getScrollY() * 6);
		window.resetScroll();



		// Cube
		shader.enable();
		glm::vec3 lightColour;
		lightColour.x = sin(count.elapsed() * 2.0f);
		lightColour.y = sin(count.elapsed() * 0.7f);
		lightColour.z = sin(count.elapsed() * 1.3f);
		glm::vec3 diffuseColour = lightColour * glm::vec3(0.5f);
		glm::vec3 ambientColour = diffuseColour * glm::vec3(0.2f);

		glm::vec3 cameraPosition = camera.getPosition();
		shader.setUniform3f("viewPos", glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z));
		shader.setUniform3f("material.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		shader.setUniform3f("material.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
		shader.setUniform3f("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		shader.setUniform1f("material.shininess", 32.0f);
		shader.setUniform3f("light.position", glm::vec3(lightPos.x, lightPos.y, lightPos.z));
		shader.setUniform3f("light.ambient", glm::vec3(ambientColour.x, ambientColour.y, ambientColour.z));
		shader.setUniform3f("light.diffuse", glm::vec3(diffuseColour.x, diffuseColour.y, diffuseColour.z));
		shader.setUniform3f("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

		glm::mat4 model(1);
		//model = glm::rotate(model, (GLfloat)count.elapsed(), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 view;
		view = camera.getViewMatrix();
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(camera.getFOV()), (float)window.getWidth() / (float)window.getHeight(), 0.1f, 100.0f);
		shader.setUniformMat4("model", model);
		shader.setUniformMat4("view", view);
		shader.setUniformMat4("projection", projection);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);


		// LightCube
		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

		lampShader.enable();
		lampShader.setUniformMat4("model", model);
		lampShader.setUniformMat4("view", view);
		lampShader.setUniformMat4("projection", projection);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		window.update();
		if (timer.elapsed() >= 1) {
			std::cout << "FPS: " << frames << std::endl;
			frames = 0;
			timer.reset();
		}
		else {
			frames++;
		}
	}
	
	return 0;
}