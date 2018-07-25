#include "Scene3D.h"

#include <iterator>
#include <iostream>
#include <glm/glm.hpp>

#include "graphics/mesh/Mesh.h"
#include "graphics/mesh/common/Cube.h"
#include "graphics/mesh/common/Sphere.h"
#include "graphics/mesh/common/Quad.h"

namespace arcane {

	Scene3D::Scene3D(graphics::Camera *camera, graphics::Window *window)
		: m_TerrainShader("src/shaders/oldBasic.vert", "src/shaders/terrain.frag"), m_ModelShader("src/shaders/basic.vert", "src/shaders/model.frag"), m_Camera(camera),
		  m_OutlineShader("src/shaders/oldBasic.vert", "src/shaders/basic.frag"), m_DynamicLightManager()
	{
		m_Renderer = new graphics::Renderer(camera);
		m_GLCache = graphics::GLCache::getInstance();
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

		// Load renderables
		graphics::Quad windowPane;
		windowPane.getMaterial().setDiffuseMap(utils::TextureLoader::Load2DTexture(std::string("res/textures/window.png")));
		windowPane.getMaterial().setSpecularMap(utils::TextureLoader::Load2DTexture(std::string("res/textures/default/fullSpec.png")));
		graphics::Model *glass = new graphics::Model(windowPane);

		//Add(new graphics::Renderable3D(glm::vec3(30.0f, -10.0f, 30.0), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 1.0f, 0.0f), 0, new arcane::graphics::Model("res/3D_Models/Overwatch/Reaper/Reaper.obj"), nullptr, false));
		//Add(new graphics::Renderable3D(glm::vec3(60.0f, -10.0f, 60.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 1.0f, 0.0f), 0, new arcane::graphics::Model("res/3D_Models/Overwatch/McCree/McCree.obj"), nullptr, true));
		//Add(new graphics::Renderable3D(glm::vec3(90.0f, -10.0f, 90.0f), glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0, new arcane::graphics::Model("res/3D_Models/Crysis/nanosuit.obj"), nullptr, false));
		Add(new graphics::Renderable3D(glm::vec3(200.0f, 200.0f, 100.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(0.0f), new arcane::graphics::Model("res/3D_Models/Sponza/sponza.obj"), nullptr, false, false));
		
		//Add(new graphics::Renderable3D(glm::vec3(40, 20, 40), glm::vec3(15, 15, 15), glm::vec3(0.0, 1.0, 0.0), glm::radians(180.0f), glass, nullptr, false, true));
		//Add(new graphics::Renderable3D(glm::vec3(80, 20, 80), glm::vec3(15, 15, 15), glm::vec3(0.0, 1.0, 0.0), glm::radians(180.0f), glass, nullptr, false, true));
		//Add(new graphics::Renderable3D(glm::vec3(120, 20, 120), glm::vec3(15, 15, 15), glm::vec3(0.0, 1.0, 0.0), glm::radians(180.0f), glass, nullptr, false, true));

		Add(new graphics::Renderable3D(glm::vec3(20, 20, 20), glm::vec3(10, 10, 10), glm::vec3(1, 0, 0), 0, new graphics::Model(graphics::Cube()), nullptr, false, false));
		Add(new graphics::Renderable3D(glm::vec3(140, 20, 140), glm::vec3(10, 10, 10), glm::vec3(1, 0, 0), 0, new graphics::Model(graphics::Sphere()), nullptr, false, false));
		Add(new graphics::Renderable3D(glm::vec3(-20, 20, -20), glm::vec3(10, 10, 10), glm::vec3(1, 0, 0), 0, new graphics::Model(graphics::Quad()), nullptr, false, false));

		// Terrain shader
		m_GLCache->switchShader(m_TerrainShader.getShaderID());
		m_TerrainShader.setUniform1f("material.shininess", 128.0f);

		// Model shader
		m_GLCache->switchShader(m_ModelShader.getShaderID());
		m_ModelShader.setUniform1f("material.shininess", 128.0f);

		// Skybox
		std::vector<const char*> skyboxFilePaths;
		skyboxFilePaths.push_back("res/skybox/right.png");
		skyboxFilePaths.push_back("res/skybox/left.png");
		skyboxFilePaths.push_back("res/skybox/top.png");
		skyboxFilePaths.push_back("res/skybox/bottom.png");
		skyboxFilePaths.push_back("res/skybox/back.png");
		skyboxFilePaths.push_back("res/skybox/front.png");
		m_Skybox = new graphics::Skybox(skyboxFilePaths, m_Camera);
	}

	void Scene3D::onUpdate(float deltaTime) {
		//m_Renderables[0]->setRadianRotation(3.14159);
	}

	void Scene3D::onRender() {
		// Setup
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(m_Camera->getFOV()), (float)graphics::Window::getWidth() / (float)graphics::Window::getHeight(), NEAR_PLANE, FAR_PLANE);
		m_DynamicLightManager.setSpotLightDirection(m_Camera->getFront());
		m_DynamicLightManager.setSpotLightPosition(m_Camera->getPosition());

		m_GLCache->switchShader(m_OutlineShader.getShaderID());
		m_OutlineShader.setUniformMat4("view", m_Camera->getViewMatrix());
		m_OutlineShader.setUniformMat4("projection", projectionMatrix);

		// Models
		m_GLCache->switchShader(m_ModelShader.getShaderID());
		m_DynamicLightManager.setupLightingUniforms(m_ModelShader);
		m_ModelShader.setUniform3f("viewPos", m_Camera->getPosition());
		m_ModelShader.setUniformMat4("view", m_Camera->getViewMatrix());
		m_ModelShader.setUniformMat4("projection", projectionMatrix);

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
		m_GLCache->setStencilWriteMask(0x00); // Don't update the stencil buffer
		m_GLCache->switchShader(m_TerrainShader.getShaderID());
		m_DynamicLightManager.setupLightingUniforms(m_TerrainShader);
		m_TerrainShader.setUniform3f("viewPos", m_Camera->getPosition());
		glm::mat4 modelMatrix(1);
		modelMatrix = glm::translate(modelMatrix, m_Terrain->getPosition());
		m_TerrainShader.setUniformMat4("model", modelMatrix);
		m_TerrainShader.setUniformMat4("view", m_Camera->getViewMatrix());
		m_TerrainShader.setUniformMat4("projection", projectionMatrix);
		m_Terrain->Draw(m_TerrainShader);

		// Skybox
		m_Skybox->Draw();

		// Transparent objects
		m_GLCache->switchShader(m_ModelShader.getShaderID());
		m_Renderer->flushTransparent(m_ModelShader, m_OutlineShader);
	}

	void Scene3D::Add(graphics::Renderable3D *renderable) {
		m_Renderables.push_back(renderable);
	}

}
