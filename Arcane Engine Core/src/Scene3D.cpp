#include "Scene3D.h"

#include <iterator>
#include <iostream>
#include <glm/glm.hpp>

namespace arcane {

	Scene3D::Scene3D(graphics::Camera *camera, graphics::Window *window)
		: m_TerrainShader("src/shaders/basic.vert", "src/shaders/terrain.frag"), m_ModelShader("src/shaders/basic.vert", "src/shaders/model.frag"), m_Camera(camera), m_Window(window),
		  m_OutlineShader("src/shaders/basic.vert", "src/shaders/basic.frag"), m_DynamicLightManager()
	{
		m_Renderer = new graphics::Renderer(camera);
		m_Terrain = new terrain::Terrain(glm::vec3(0.0f, -20.0f, 0.0f));

		init();
	}

	Scene3D::~Scene3D() {

	}

	void Scene3D::init() {
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_CULL_FACE);

		// Load models
		std::vector<graphics::Mesh> meshes;
		meshes.push_back(*m_meshFactory.CreateQuad("res/textures/window.png", false));

		Add(new graphics::Renderable3D(glm::vec3(30.0f, -10.0f, 30.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 1.0f, 0.0f), 0, new arcane::graphics::Model("res/3D_Models/Overwatch/Reaper/Reaper.obj"), nullptr, true));
		Add(new graphics::Renderable3D(glm::vec3(60.0f, -10.0f, 60.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 1.0f, 0.0f), 0, new arcane::graphics::Model("res/3D_Models/Overwatch/McCree/McCree.obj"), nullptr, false));
		//Add(new graphics::Renderable3D(glm::vec3(90.0f, -10.0f, 90.0f), glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0, new arcane::graphics::Model("res/3D_Models/Crysis/nanosuit.obj"), true));
		//Add(new graphics::Renderable3D(glm::vec3(200.0f, 200.0f, 100.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(0.0f), new arcane::graphics::Model("res/3D_Models/Sponza/sponza.obj")));
		Add(new graphics::Renderable3D(glm::vec3(40, 20, 40), glm::vec3(15, 15, 15), glm::vec3(1.0, 0.0, 0.0), glm::radians(90.0f), new graphics::Model(meshes), nullptr, false, true));
		Add(new graphics::Renderable3D(glm::vec3(80, 20, 80), glm::vec3(15, 15, 15), glm::vec3(1.0, 0.0, 0.0), glm::radians(90.0f), new graphics::Model(meshes), nullptr, false, true));
		Add(new graphics::Renderable3D(glm::vec3(120, 20, 120), glm::vec3(15, 15, 15), glm::vec3(1.0, 0.0, 0.0), glm::radians(90.0f), new graphics::Model(meshes), nullptr, false, true));

		// Terrain shader
		m_TerrainShader.enable();
		m_TerrainShader.setUniform1f("material.shininess", 128.0f);
		m_TerrainShader.setUniform3f("dirLight.direction", glm::vec3(-0.3f, -1.0f, -0.3f));
		m_TerrainShader.setUniform3f("dirLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
		m_TerrainShader.setUniform3f("dirLight.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
		m_TerrainShader.setUniform3f("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		m_TerrainShader.setUniform3f("spotLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		m_TerrainShader.setUniform3f("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		m_TerrainShader.setUniform3f("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		m_TerrainShader.setUniform1f("spotLight.constant", 1.0f);
		m_TerrainShader.setUniform1f("spotLight.linear", 0.022);
		m_TerrainShader.setUniform1f("spotLight.quadratic", 0.0019);
		m_TerrainShader.setUniform1f("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		m_TerrainShader.setUniform1f("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
		m_TerrainShader.setUniform3f("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		m_TerrainShader.setUniform3f("pointLights[0].diffuse", glm::vec3(0.85f, 0.85f, 0.85f));
		m_TerrainShader.setUniform3f("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		m_TerrainShader.setUniform1f("pointLights[0].constant", 1.0f);
		m_TerrainShader.setUniform1f("pointLights[0].linear", 0.007);
		m_TerrainShader.setUniform1f("pointLights[0].quadratic", 0.0002);

		// Model shader
		m_ModelShader.enable();
		m_ModelShader.setUniform1f("material.shininess", 128.0f);
		m_ModelShader.setUniform3f("dirLight.direction", glm::vec3(-0.3f, -1.0f, -0.3f));
		m_ModelShader.setUniform3f("dirLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
		m_ModelShader.setUniform3f("dirLight.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
		m_ModelShader.setUniform3f("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		m_ModelShader.setUniform3f("spotLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		m_ModelShader.setUniform3f("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		m_ModelShader.setUniform3f("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		m_ModelShader.setUniform1f("spotLight.constant", 1.0f);
		m_ModelShader.setUniform1f("spotLight.linear", 0.022);
		m_ModelShader.setUniform1f("spotLight.quadratic", 0.0019);
		m_ModelShader.setUniform1f("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		m_ModelShader.setUniform1f("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
		m_ModelShader.setUniform3f("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		m_ModelShader.setUniform3f("pointLights[0].diffuse", glm::vec3(0.85f, 0.85f, 0.85f));
		m_ModelShader.setUniform3f("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		m_ModelShader.setUniform1f("pointLights[0].constant", 1.0f);
		m_ModelShader.setUniform1f("pointLights[0].linear", 0.007);
		m_ModelShader.setUniform1f("pointLights[0].quadratic", 0.0002);

		// Skybox
		std::vector<const char*> skyboxFilePaths;
		skyboxFilePaths.push_back("res/skybox/right.png");
		skyboxFilePaths.push_back("res/skybox/left.png");
		skyboxFilePaths.push_back("res/skybox/top.png");
		skyboxFilePaths.push_back("res/skybox/bottom.png");
		skyboxFilePaths.push_back("res/skybox/back.png");
		skyboxFilePaths.push_back("res/skybox/front.png");
		m_Skybox = new graphics::Skybox(skyboxFilePaths, m_Camera, m_Window);
	}

	void Scene3D::onUpdate(float deltaTime) {
		//m_Renderables[0]->setRadianRotation(3.14159);
	}

	void Scene3D::onRender() {
		// Setup
		m_DynamicLightManager.setSpotLightDirection(m_Camera->getFront());
		m_DynamicLightManager.setSpotLightPosition(m_Camera->getPosition());

		m_OutlineShader.enable();
		m_OutlineShader.setUniformMat4("view", m_Camera->getViewMatrix());
		m_OutlineShader.setUniformMat4("projection", glm::perspective(glm::radians(m_Camera->getFOV()), (float)m_Window->getWidth() / (float)m_Window->getHeight(), 0.1f, 1000.0f));

		// Models
		m_ModelShader.enable();
		m_ModelShader.setUniform3f("pointLights[0].position", glm::vec3(200.0f, 215.0f, 100.0f));
		m_ModelShader.setUniform3f("spotLight.position", m_Camera->getPosition());
		m_ModelShader.setUniform3f("spotLight.direction", m_Camera->getFront());
		m_ModelShader.setUniform3f("viewPos", m_Camera->getPosition());
		m_ModelShader.setUniformMat4("view", m_Camera->getViewMatrix());
		m_ModelShader.setUniformMat4("projection", glm::perspective(glm::radians(m_Camera->getFOV()), (float)m_Window->getWidth() / (float)m_Window->getHeight(), 0.1f, 1000.0f));

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

		m_Renderer->flushOpaque(m_ModelShader, m_OutlineShader);

		// Terrain
		glStencilMask(0x00); // Don't update the stencil buffer
		glEnable(GL_CULL_FACE);
		m_TerrainShader.enable();
		m_TerrainShader.setUniform3f("pointLights[0].position", glm::vec3(200.0f, 200.0f, 100.0f));
		m_TerrainShader.setUniform3f("spotLight.position", m_Camera->getPosition());
		m_TerrainShader.setUniform3f("spotLight.direction", m_Camera->getFront());
		m_TerrainShader.setUniform3f("viewPos", m_Camera->getPosition());
		glm::mat4 modelMatrix(1);
		modelMatrix = glm::translate(modelMatrix, m_Terrain->getPosition());
		m_TerrainShader.setUniformMat4("model", modelMatrix);
		m_TerrainShader.setUniformMat4("view", m_Camera->getViewMatrix());
		m_TerrainShader.setUniformMat4("projection", glm::perspective(glm::radians(m_Camera->getFOV()), (float)m_Window->getWidth() / (float)m_Window->getHeight(), 0.1f, 1000.0f));
		m_Terrain->Draw(m_TerrainShader);

		// Skybox
		m_Skybox->Draw();

		// Transparent objects
		m_ModelShader.enable();
		m_Renderer->flushTransparent(m_ModelShader, m_OutlineShader);
	}

	void Scene3D::Add(graphics::Renderable3D *renderable) {
		m_Renderables.push_back(renderable);
	}

}
