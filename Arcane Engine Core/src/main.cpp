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
#include <ft2build.h>

arcane::graphics::FPSCamera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
arcane::graphics::Window window("Arcane Engine", 1366, 768);
GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;

int main() {
	glEnable(GL_DEPTH_TEST);

	//arcane::graphics::Shader shader("src/shaders/basic.vert", "src/shaders/multipleLight.frag");
	arcane::graphics::Shader shader("src/shaders/basic.vert", "src/shaders/terrain.frag");
	arcane::graphics::Shader lampShader("src/shaders/basic.vert", "src/shaders/lightCube.frag");

	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		// Positions         // Normals           // Texture Coords (UV Mapping)
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.0f, -30.0f, 0.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	// Note: When calling glVertexAttribPointer, it uses whatever GL_ARRAY_BUFFER is set to at the time to read its data from
	GLuint VBO, lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//// Light Cube
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); // Enable them so they are used when drawing
	glBindVertexArray(0); // Unbind VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO

	//// Load Textures
	//GLuint diffuseMap, specularMap, emissionMap;
	//glGenTextures(1, &diffuseMap);
	//glGenTextures(1, &specularMap);
	//glGenTextures(1, &emissionMap);
	//int width, height;
	//unsigned char *image;

	// Diffuse map
	//image = SOIL_load_image("res//container2.png", &width, &height, 0, SOIL_LOAD_RGB);
	//glBindTexture(GL_TEXTURE_2D, diffuseMap);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image); // Load our diffuse map
	//glGenerateMipmap(GL_TEXTURE_2D); // Generate mip maps for what is currently bounded to GL_TEXTURE_2D
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//SOIL_free_image_data(image);

	//// Specular map
	//image = SOIL_load_image("res//container2_specular.png", &width, &height, 0, SOIL_LOAD_RGB);
	//glBindTexture(GL_TEXTURE_2D, specularMap);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//glGenerateMipmap(GL_TEXTURE_2D); // Generate mip maps for what is currently bounded to GL_TEXTURE_2D
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//SOIL_free_image_data(image);

	//// Emission map
	//image = SOIL_load_image("res//container2_emission.png", &width, &height, 0, SOIL_LOAD_RGB);
	//glBindTexture(GL_TEXTURE_2D, emissionMap);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//glGenerateMipmap(GL_TEXTURE_2D); // Generate mip maps for what is currently bounded to GL_TEXTURE_2D
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//SOIL_free_image_data(image);

	arcane::terrain::Terrain terrain(glm::vec3(-350, -40, -350));

	// Load model
	std::string test = "res/3D_Models/Crysis/nanosuit.obj";
	//std::string test = "res/3D_Models/Sponza/sponza.obj";
	//std::string test = "res/3D_Models/Town/Buildings.obj";
	//arcane::graphics::Model nanosuitModel(test.c_str());

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Prepare the fps counter right before the first tick
	arcane::Timer timer;
	int frames = 0;

	// Temp Rotation Timer
	arcane::Timer count;
	arcane::Time time;

	bool firstMove = true;
	GLfloat lastX = window.getMouseX();
	GLfloat lastY = window.getMouseY();
	while (!window.closed()) {
		glClearColor(0.26f, 0.95f, 0.9f, 1.0f); // Scene background colour
		window.clear();
		time.update();

		// Check to see if the mouse hasn't been moved yet
		if (firstMove && (lastX != window.getMouseX() || lastY != window.getMouseY())) {
			lastX = window.getMouseX();
			lastY = window.getMouseY();
			firstMove = false;
		}

		// Camera Processes
		camera.processMouseMovement(window.getMouseX() - lastX, lastY - window.getMouseY(), true);
		lastX = window.getMouseX();
		lastY = window.getMouseY();

		if (window.isKeyPressed(GLFW_KEY_W))
			camera.processKeyboard(arcane::graphics::FORWARD, time.getDeltaTime());
		if (window.isKeyPressed(GLFW_KEY_S))
			camera.processKeyboard(arcane::graphics::BACKWARD, time.getDeltaTime());
		if (window.isKeyPressed(GLFW_KEY_A))
			camera.processKeyboard(arcane::graphics::LEFT, time.getDeltaTime());
		if (window.isKeyPressed(GLFW_KEY_D))
			camera.processKeyboard(arcane::graphics::RIGHT, time.getDeltaTime());
		if (window.isKeyPressed(GLFW_KEY_SPACE))
			camera.processKeyboard(arcane::graphics::UPWARDS, time.getDeltaTime());
		if (window.isKeyPressed(GLFW_KEY_LEFT_CONTROL))
			camera.processKeyboard(arcane::graphics::DOWNWARDS, time.getDeltaTime());

		camera.processMouseScroll(window.getScrollY() * 6);
		window.resetScroll();

		shader.enable();
		glm::vec3 cameraPosition = camera.getPosition();
		shader.setUniform3f("viewPos", glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z));
		shader.setUniform1f("material.shininess", 32.0f);

		//// directional light
		shader.setUniform3f("dirLight.direction", glm::vec3(0.0f, -1.0f, 0.0f));
		shader.setUniform3f("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		shader.setUniform3f("dirLight.diffuse", glm::vec3(0.1f, 0.1f, 0.1f));
		shader.setUniform3f("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

		//// point lights
		shader.setUniform3f("pointLight.position", pointLightPositions[0] + glm::vec3(sin(glfwGetTime()) * 30, 0.0f, cos(glfwGetTime()) * 30));
		shader.setUniform3f("pointLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		shader.setUniform3f("pointLight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		shader.setUniform3f("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		shader.setUniform1f("pointLight.constant", 1.0f);
		shader.setUniform1f("pointLight.linear", 0.007);
		shader.setUniform1f("pointLight.quadratic", 0.0002);

		//shader.setUniform3f("pointLights[0].position", pointLightPositions[0]);
		//shader.setUniform3f("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		//shader.setUniform3f("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		//shader.setUniform3f("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		//shader.setUniform1f("pointLights[0].constant", 1.0f);
		//shader.setUniform1f("pointLights[0].linear", 0.09);
		//shader.setUniform1f("pointLights[0].quadratic", 0.032);

		//shader.setUniform3f("pointLights[0].position", pointLightPositions[0]);
		//shader.setUniform3f("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		//shader.setUniform3f("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		//shader.setUniform3f("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		//shader.setUniform1f("pointLights[0].constant", 1.0f);
		//shader.setUniform1f("pointLights[0].linear", 0.09);
		//shader.setUniform1f("pointLights[0].quadratic", 0.032);
		//// point light 2
		//shader.setUniform3f("pointLights[1].position", pointLightPositions[1]);
		//shader.setUniform3f("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		//shader.setUniform3f("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		//shader.setUniform3f("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		//shader.setUniform1f("pointLights[1].constant", 1.0f);
		//shader.setUniform1f("pointLights[1].linear", 0.09);
		//shader.setUniform1f("pointLights[1].quadratic", 0.032);
		//// point light 3
		//shader.setUniform3f("pointLights[2].position", pointLightPositions[2]);
		//shader.setUniform3f("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		//shader.setUniform3f("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		//shader.setUniform3f("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		//shader.setUniform1f("pointLights[2].constant", 1.0f);
		//shader.setUniform1f("pointLights[2].linear", 0.09);
		//shader.setUniform1f("pointLights[2].quadratic", 0.032);
		//// point light 4
		//shader.setUniform3f("pointLights[3].position", pointLightPositions[3]);
		//shader.setUniform3f("pointLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		//shader.setUniform3f("pointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		//shader.setUniform3f("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		//shader.setUniform1f("pointLights[3].constant", 1.0f);
		//shader.setUniform1f("pointLights[3].linear", 0.09);
		//shader.setUniform1f("pointLights[3].quadratic", 0.032);

		//// spotLight
		shader.setUniform3f("spotLight.position", camera.getPosition());
		shader.setUniform3f("spotLight.direction", camera.getFront());
		shader.setUniform3f("spotLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		shader.setUniform3f("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		shader.setUniform3f("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		shader.setUniform1f("spotLight.constant", 1.0f);
		shader.setUniform1f("spotLight.linear", 0.022);
		shader.setUniform1f("spotLight.quadratic", 0.0019);
		shader.setUniform1f("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		shader.setUniform1f("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		// Draw Terrain
		shader.enable();
		glm::mat4 view;
		view = camera.getViewMatrix();
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(camera.getFOV()), (float)window.getWidth() / (float)window.getHeight(), 0.1f, 1000.0f);
		shader.setUniformMat4("view", view);
		shader.setUniformMat4("projection", projection);
		shader.setUniform1f("time", glfwGetTime());

		terrain.Draw(shader);

		//// Draw model
		//glm::mat4 model;
		//model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 10.0f));
		//shader.setUniformMat4("model", model);
		//nanosuitModel.Draw(shader);


		glBindVertexArray(lightVAO);
		lampShader.enable();
		lampShader.setUniformMat4("view", view);
		lampShader.setUniformMat4("projection", projection);
		// LightCube
		for (unsigned int i = 0; i < 1; ++i) {
			glm::mat4 model = glm::mat4();
			model = glm::translate(model, pointLightPositions[i] + glm::vec3(sin(glfwGetTime()) * 30, 0.0f, cos(glfwGetTime()) * 30));
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
			lampShader.setUniformMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
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

	// Clean up the memory (quite useless in this spot)
	/*glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);*/

	return 0;
}