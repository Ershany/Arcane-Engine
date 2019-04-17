#include "pch.h"
#include "Scene3D.h"

#include <graphics/mesh/Mesh.h>
#include <graphics/mesh/common/Cube.h>
#include <graphics/mesh/common/Sphere.h>
#include <graphics/mesh/common/Quad.h>
#include <pathfinding/PathfindingAgent.h>

namespace arcane {

	Scene3D::Scene3D(Window *window)
		: m_SceneCamera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f), m_ModelRenderer(getCamera()), m_Terrain(glm::vec3(0.0f, 0.0f, 0.0f)), m_ProbeManager(m_SceneProbeBlendSetting)
	{
		m_GLCache = GLCache::getInstance();

		init();
	}

	Scene3D::~Scene3D() {
		
	}

	void Scene3D::init() {
		m_GLCache->setMultisample(true);

		// Load renderables
		RenderableModel *agentsRenderable = new RenderableModel(glm::vec3(50.0f, 0.0f, 50.0f), glm::vec3(3.0f, 10.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, new Model(Cube()), nullptr);
		m_Agent = new PathfindingAgent(&m_Terrain, agentsRenderable);
		m_RenderableModels.push_back(agentsRenderable);

		// Skybox
		std::vector<std::string> skyboxFilePaths;
		skyboxFilePaths.push_back("res/skybox/right.png");
		skyboxFilePaths.push_back("res/skybox/left.png");
		skyboxFilePaths.push_back("res/skybox/top.png");
		skyboxFilePaths.push_back("res/skybox/bottom.png");
		skyboxFilePaths.push_back("res/skybox/back.png");
		skyboxFilePaths.push_back("res/skybox/front.png");
		m_Skybox = new Skybox(skyboxFilePaths);
		m_ProbeManager.init(m_Skybox);
	}

	void Scene3D::onUpdate(float deltaTime) {
		// Camera Update
		m_SceneCamera.processInput(deltaTime);

		// Entity Update
		m_Agent->update(deltaTime);

		m_DynamicLightManager.setSpotLightDirection(m_SceneCamera.getFront());
		m_DynamicLightManager.setSpotLightPosition(m_SceneCamera.getPosition());
	}

	void Scene3D::onRender() {

	}

	void Scene3D::addModelsToRenderer() {
		auto iter = m_RenderableModels.begin();
		while (iter != m_RenderableModels.end()) {
			RenderableModel *curr = *iter;
			if (curr->getTransparent()) {
				m_ModelRenderer.submitTransparent(curr);
			}
			else {
				m_ModelRenderer.submitOpaque(curr);
			}

			iter++;
		}
	}

}
