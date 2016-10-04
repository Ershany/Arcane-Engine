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

	// Make and Enable a Shader
	arcane::graphics::Shader shader("src/shaders/basic.vert", "src/shaders/basic.frag");
	shader.enable();

	GLuint texture1, texture2;
	glGenTextures(1, &texture1);
	glGenTextures(1, &texture2);
	int width, height;

	glBindTexture(GL_TEXTURE_2D, texture1);
	unsigned char *image = SOIL_load_image("res//container.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture2);
	unsigned char *image2 = SOIL_load_image("res//awesomeface.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,	 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,	     1.0f, 0.0f,	0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f,		 1.0f, 1.0f,	0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f,		 1.0f, 1.0f,	0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, -0.5f,	     0.0f, 1.0f,	1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f,	     0.0f, 0.0f,	1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f,	     1.0f, 0.0f,	0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f,	     1.0f, 1.0f,	0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f,	     1.0f, 1.0f,	0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f,	     0.0f, 1.0f,	1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,	     0.0f, 0.0f,	1.0f, 0.0f, 0.0f,

		-0.5f, 0.5f, 0.5f,	     1.0f, 0.0f,	1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f,		 1.0f, 1.0f,	0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f,	     0.0f, 0.0f,	1.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f,	     1.0f, 0.0f,	1.0f, 0.0f, 0.0f,

		0.5f, 0.5f, 0.5f,	     1.0f, 0.0f,	1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f,	     1.0f, 1.0f,	0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f,		 0.0f, 0.0f,	1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f,		 1.0f, 0.0f,	1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,	 0.0f, 1.0f,	1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		 1.0f, 1.0f,	0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f,		 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f,		 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f,		 0.0f, 0.0f,	1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	 0.0f, 1.0f,	1.0f, 0.0f, 0.0f,

		-0.5f, 0.5f, -0.5f,	     0.0f, 1.0f,	1.0f, 0.0f, 0.0f, 
		0.5f, 0.5f, -0.5f,		 1.0f, 1.0f,	0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f,		 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f,		 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f,		 0.0f, 0.0f,	1.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f,		 0.0f, 1.0f,	1.0f, 0.0f, 0.0f,
	};

	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Colour attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Prepare the fps counter right before the first tick
	arcane::Timer timer;
	int frames = 0;

	// Temp Rotation Timer
	arcane::Timer count;
	
	arcane::Time time;

	lastX = window.getMouseX();
	lastY = window.getMouseY();
	while (!window.closed()) {
		window.clear();
		time.update();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform1i(glGetUniformLocation(shader.getShaderID(), "ourTexture1"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniform1i(glGetUniformLocation(shader.getShaderID(), "ourTexture1"), 1);


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
		
		camera.processMouseScroll(window.getScrollY());
		

		glm::mat4 model(1);
		//model = model.rotation(count.elapsed() * 15, vec3(1.0f, 0.0f, 0.0f));
		//model = model * model.rotation(count.elapsed() * 15, vec3(0.0f, 1.0f, 0.0f));
		
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