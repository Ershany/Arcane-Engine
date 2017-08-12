#include "Scene3D.h"

#include <iterator>
#include <iostream>
#include <glm/glm.hpp>

namespace arcane {

	Scene3D::Scene3D(graphics::FPSCamera *camera, graphics::Window *window)
		: terrainShader("src/shaders/basic.vert", "src/shaders/terrain.frag"), modelShader("src/shaders/basic.vert", "src/shaders/model.frag"), m_Camera(camera), m_Window(window),
		  outlineShader("src/shaders/basic.vert", "src/shaders/basic.frag")
	{
		m_Renderer = new graphics::Renderer(camera);
		m_Terrain = new terrain::Terrain(glm::vec3(0.0f, -20.0f, 0.0f));

		init();
	}

	Scene3D::~Scene3D() {

	}

	void Scene3D::init() {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		//glEnable(GL_CULL_FACE);

		// Load models
		std::vector<graphics::Mesh> meshes;
		meshes.push_back(*m_meshFactory.CreateQuad("res/textures/window.png", false));

		Add(new graphics::Renderable3D(glm::vec3(30.0f, -10.0f, 30.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 1.0f, 0.0f), 0, new arcane::graphics::Model("res/3D_Models/Overwatch/Reaper/Reaper.obj"), false));
		Add(new graphics::Renderable3D(glm::vec3(60.0f, -10.0f, 60.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 1.0f, 0.0f), 0, new arcane::graphics::Model("res/3D_Models/Overwatch/McCree/McCree.obj"), false));
		//Add(new graphics::Renderable3D(glm::vec3(90.0f, -10.0f, 90.0f), glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0, new arcane::graphics::Model("res/3D_Models/Crysis/nanosuit.obj"), false));
		//Add(new graphics::Renderable3D(glm::vec3(200.0f, 200.0f, 100.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 0.0f, 0.0f), 0, new arcane::graphics::Model("res/3D_Models/Sponza/sponza.obj"), true));
		Add(new graphics::Renderable3D(glm::vec3(40, 20, 40), glm::vec3(15, 15, 15), glm::vec3(1.0, 0.0, 0.0), glm::radians(90.0f), new graphics::Model(meshes), false, true));
		Add(new graphics::Renderable3D(glm::vec3(80, 20, 80), glm::vec3(15, 15, 15), glm::vec3(1.0, 0.0, 0.0), glm::radians(90.0f), new graphics::Model(meshes), false, true));
		Add(new graphics::Renderable3D(glm::vec3(120, 20, 120), glm::vec3(15, 15, 15), glm::vec3(1.0, 0.0, 0.0), glm::radians(90.0f), new graphics::Model(meshes), false, true));

		// Terrain shader
		terrainShader.enable();
		terrainShader.setUniform1f("material.shininess", 128.0f);
		terrainShader.setUniform3f("dirLight.direction", glm::vec3(-0.3f, -1.0f, -0.3f));
		terrainShader.setUniform3f("dirLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
		terrainShader.setUniform3f("dirLight.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
		terrainShader.setUniform3f("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		terrainShader.setUniform3f("spotLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		terrainShader.setUniform3f("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		terrainShader.setUniform3f("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		terrainShader.setUniform1f("spotLight.constant", 1.0f);
		terrainShader.setUniform1f("spotLight.linear", 0.022);
		terrainShader.setUniform1f("spotLight.quadratic", 0.0019);
		terrainShader.setUniform1f("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		terrainShader.setUniform1f("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
		terrainShader.setUniform3f("pointLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		terrainShader.setUniform3f("pointLight.diffuse", glm::vec3(0.85f, 0.85f, 0.85f));
		terrainShader.setUniform3f("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		terrainShader.setUniform1f("pointLight.constant", 1.0f);
		terrainShader.setUniform1f("pointLight.linear", 0.007);
		terrainShader.setUniform1f("pointLight.quadratic", 0.0002);

		// Model shader
		modelShader.enable();
		modelShader.setUniform1f("material.shininess", 128.0f);
		modelShader.setUniform3f("dirLight.direction", glm::vec3(-0.3f, -1.0f, -0.3f));
		modelShader.setUniform3f("dirLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
		modelShader.setUniform3f("dirLight.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
		modelShader.setUniform3f("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		modelShader.setUniform3f("spotLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		modelShader.setUniform3f("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		modelShader.setUniform3f("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		modelShader.setUniform1f("spotLight.constant", 1.0f);
		modelShader.setUniform1f("spotLight.linear", 0.022);
		modelShader.setUniform1f("spotLight.quadratic", 0.0019);
		modelShader.setUniform1f("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		modelShader.setUniform1f("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
		modelShader.setUniform3f("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		modelShader.setUniform3f("pointLights[0].diffuse", glm::vec3(0.85f, 0.85f, 0.85f));
		modelShader.setUniform3f("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		modelShader.setUniform1f("pointLights[0].constant", 1.0f);
		modelShader.setUniform1f("pointLights[0].linear", 0.007);
		modelShader.setUniform1f("pointLights[0].quadratic", 0.0002);
	}

	void Scene3D::onUpdate(float deltaTime) {
		//m_Renderables[0]->setRadianRotation(m_Renderables[0]->getRadianRotation() + deltaTime);
	}

	void Scene3D::onRender() {
		// Setup
		outlineShader.enable();
		outlineShader.setUniformMat4("view", m_Camera->getViewMatrix());
		outlineShader.setUniformMat4("projection", glm::perspective(glm::radians(m_Camera->getFOV()), (float)m_Window->getWidth() / (float)m_Window->getHeight(), 0.1f, 1000.0f));

		// Terrain
		glStencilMask(0x00); // Don't update the stencil buffer
		terrainShader.enable();
		terrainShader.setUniform3f("pointLight.position", glm::vec3(200.0f, 200.0f, 100.0f));
		terrainShader.setUniform3f("spotLight.position", m_Camera->getPosition());
		terrainShader.setUniform3f("spotLight.direction", m_Camera->getFront());
		terrainShader.setUniform3f("viewPos", m_Camera->getPosition());
		glm::mat4 modelMatrix(1);
		modelMatrix = glm::translate(modelMatrix, m_Terrain->getPosition());
		terrainShader.setUniformMat4("model", modelMatrix);
		terrainShader.setUniformMat4("view", m_Camera->getViewMatrix());
		terrainShader.setUniformMat4("projection", glm::perspective(glm::radians(m_Camera->getFOV()), (float)m_Window->getWidth() / (float)m_Window->getHeight(), 0.1f, 1000.0f));
		m_Terrain->Draw(terrainShader);

		// Models
		modelShader.enable();
		modelShader.setUniform3f("pointLights[0].position", glm::vec3(200.0f, 215.0f, 100.0f));
		modelShader.setUniform3f("spotLight.position", m_Camera->getPosition());
		modelShader.setUniform3f("spotLight.direction", m_Camera->getFront());
		modelShader.setUniform3f("viewPos", m_Camera->getPosition());
		modelShader.setUniformMat4("view", m_Camera->getViewMatrix());
		modelShader.setUniformMat4("projection", glm::perspective(glm::radians(m_Camera->getFOV()), (float)m_Window->getWidth() / (float)m_Window->getHeight(), 0.1f, 1000.0f));
		
		std::vector<graphics::Renderable3D*>::iterator iter = m_Renderables.begin();
		while (iter != m_Renderables.end()) {
			graphics::Renderable3D *curr = *iter;
			if (curr->getTransparent()) {
				m_Renderer->submitTransparent(curr);
			}
			else {
				m_Renderer->submitOpaque(curr);
			}
			
			iter++;
		}
		m_Renderer->flush(modelShader, outlineShader);
	}

	void Scene3D::Add(graphics::Renderable3D *renderable) {
		m_Renderables.push_back(renderable);
	}

}
