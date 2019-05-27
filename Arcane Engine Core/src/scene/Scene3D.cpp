#include "pch.h"
#include "Scene3D.h"

#include <graphics/mesh/Mesh.h>
#include <graphics/mesh/common/Cube.h>
#include <graphics/mesh/common/Sphere.h>
#include <graphics/mesh/common/Quad.h>

namespace arcane {

	Scene3D::Scene3D(Window *window)
		: m_SceneCamera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f), m_ModelRenderer(getCamera()), m_Terrain(glm::vec3(0.0f, -20.0f, 0.0f)), m_ProbeManager(m_SceneProbeBlendSetting)
	{
		m_GLCache = GLCache::getInstance();

		init();
	}

	Scene3D::~Scene3D() {
		
	}

	void Scene3D::init() {
		m_GLCache->setMultisample(true);

		// Load renderables
		/*
		Quad windowPane;
		windowPane.getMaterial().setDiffuseMap(TextureLoader::load2DTexture(std::string("res/textures/window.png"), true));
		windowPane.getMaterial().setSpecularMap(TextureLoader::load2DTexture(std::string("res/textures/default/fullSpec.png"), false));
		Model *glass = new Model(windowPane);
		*/

		//m_RenderableModels.push_back(new RenderableModel(glm::vec3(200.0f, 50.0f, 100.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(0.0f), new arcane::Model("res/3D_Models/Sponza/sponza.obj"), nullptr, true, false));
		
		//m_RenderableModels.push_back(new RenderableModel(glm::vec3(40, 60, 40), glm::vec3(15, 15, 15), glm::vec3(0.0, 1.0, 0.0), glm::radians(180.0f), glass, nullptr, true, true));
		//m_RenderableModels.push_back(new RenderableModel(glm::vec3(80, 60, 80), glm::vec3(15, 15, 15), glm::vec3(0.0, 1.0, 0.0), glm::radians(180.0f), glass, nullptr, true, true));
		//m_RenderableModels.push_back(new RenderableModel(glm::vec3(120, 60, 120), glm::vec3(15, 15, 15), glm::vec3(0.0, 1.0, 0.0), glm::radians(180.0f), glass, nullptr, true, true));

		//m_RenderableModels.push_back(new RenderableModel(glm::vec3(20, 90, 20), glm::vec3(10, 10, 10), glm::vec3(1, 0, 0), 0, new Model(Cube()), nullptr, false, false));
		//m_RenderableModels.push_back(new RenderableModel(glm::vec3(140, 90, 140), glm::vec3(10, 10, 10), glm::vec3(1, 0, 0), 0, new Model(Sphere()), nullptr, false, false));
		//m_RenderableModels.push_back(new RenderableModel(glm::vec3(-20, 90, -20), glm::vec3(10, 10, 10), glm::vec3(1, 0, 0), 0, new Model(Quad()), nullptr, false, false));

		// Temp code until I rewrite the model loader
		Model *pbrGun = new arcane::Model("res/3D_Models/Cerberus_Gun/Cerberus_LP.FBX");
		m_RenderableModels.push_back(new RenderableModel(glm::vec3(120.0f, 75.0f, 120.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(-90.0f), pbrGun, nullptr, false, false));
		//pbrGun->getMeshes()[0].getMaterial().setAlbedoMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_A.tga"), true));
		//pbrGun->getMeshes()[0].getMaterial().setNormalMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_N.tga"), false));
		//pbrGun->getMeshes()[0].getMaterial().setMetallicMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_M.tga"), false));
		//pbrGun->getMeshes()[0].getMaterial().setRoughnessMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_R.tga"), false));
		//pbrGun->getMeshes()[0].getMaterial().setAmbientOcclusionMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_AO.tga"), false));

		// Temp testing code
		int nrRows = 1;
		int nrColumns = 1;
		float spacing = 2.5;
		for (int row = 0; row < nrRows; row++) {
			for (int col = 0; col < nrColumns; col++) {
				Model *sphere = new arcane::Model("res/3D_Models/Sphere/globe-sphere.obj");
				Material &mat = sphere->getMeshes()[0].getMaterial();
				//mat.setAlbedoMap(TextureLoader::getDefaultAO());
				mat.setNormalMap(TextureLoader::getDefaultNormal());
				mat.setAmbientOcclusionMap(TextureLoader::getDefaultAO());
				mat.setMetallicMap(TextureLoader::getFullMetallic());
				mat.setRoughnessMap(TextureLoader::getNoRoughness());
				m_RenderableModels.push_back(new RenderableModel(glm::vec3((float)(col - (nrColumns / 2)) * spacing + 60,
					(float)(row - (nrRows / 2)) * spacing + 90, 130.0f), glm::vec3(20.0f, 20.0f, 20.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.0f, sphere, nullptr, false, false));
			}
		}

		// Skybox
		std::vector<std::string> skyboxFilePaths;
		skyboxFilePaths.push_back("res/skybox/right.png");
		skyboxFilePaths.push_back("res/skybox/left.png");
		skyboxFilePaths.push_back("res/skybox/top.png");
		skyboxFilePaths.push_back("res/skybox/bottom.png");
		skyboxFilePaths.push_back("res/skybox/back.png");
		skyboxFilePaths.push_back("res/skybox/front.png");
		m_Skybox = new Skybox(skyboxFilePaths);
	}

	void Scene3D::onUpdate(float deltaTime) {
		// Camera Update
		m_SceneCamera.processInput(deltaTime);

		m_DynamicLightManager.setSpotLightDirection(0, m_SceneCamera.getFront());
		m_DynamicLightManager.setSpotLightPosition(0, m_SceneCamera.getPosition());
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
