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
		TextureSettings srgbTextureSettings;
		srgbTextureSettings.IsSRGB = true;

		Model *pbrGun = new arcane::Model("res/3D_Models/Cerberus_Gun/Cerberus_LP.FBX");
		m_RenderableModels.push_back(new RenderableModel(glm::vec3(815.0f, 150.0f, 1242.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(-90.0f), pbrGun, nullptr, true, false));
		//pbrGun->getMeshes()[0].getMaterial().setAlbedoMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_A.tga"), &srgbTextureSettings));
		//pbrGun->getMeshes()[0].getMaterial().setNormalMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_N.tga")));
		//pbrGun->getMeshes()[0].getMaterial().setMetallicMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_M.tga")));
		//pbrGun->getMeshes()[0].getMaterial().setRoughnessMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_R.tga")));
		//pbrGun->getMeshes()[0].getMaterial().setAmbientOcclusionMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_AO.tga")));

		//Model *hyruleShield = new arcane::Model("res/3D_Models/Hyrule_Shield/HShield.obj");
		//m_RenderableModels.push_back(new RenderableModel(glm::vec3(67.0f, 92.0f, 133.0f), glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.0f), hyruleShield, nullptr, false, false));
		//hyruleShield->getMeshes()[0].getMaterial().setAlbedoMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Hyrule_Shield/HShield_[Albedo].tga"), &srgbTextureSettings));
		//hyruleShield->getMeshes()[0].getMaterial().setNormalMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Hyrule_Shield/HShield_[Normal].tga")));
		//hyruleShield->getMeshes()[0].getMaterial().setMetallicMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Hyrule_Shield/HShield_[Metallic].tga")));
		//hyruleShield->getMeshes()[0].getMaterial().setRoughnessMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Hyrule_Shield/HShield_[Roughness].tga")));
		//hyruleShield->getMeshes()[0].getMaterial().setAmbientOcclusionMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Hyrule_Shield/HShield_[Occlusion].tga")));

		//Model *sponza = new arcane::Model("res/3D_Models/Sponza/sponza.obj");
		//m_RenderableModels.push_back(new RenderableModel(glm::vec3(67.0f, 110.0f, 133.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.0f), sponza, nullptr, true, false));

		// Skybox
		std::vector<std::string> skyboxFilePaths;
		skyboxFilePaths.push_back("res/skybox/right.png");
		skyboxFilePaths.push_back("res/skybox/left.png");
		skyboxFilePaths.push_back("res/skybox/top.png");
		skyboxFilePaths.push_back("res/skybox/bottom.png");
		skyboxFilePaths.push_back("res/skybox/back.png");
		skyboxFilePaths.push_back("res/skybox/front.png");
		m_Skybox = new Skybox(skyboxFilePaths);

		m_SceneCamera.setPosition(glm::vec3(90.0f, 80.0f, 180.0f));
	}

	void Scene3D::onUpdate(float deltaTime) {
		// Camera Update
		m_SceneCamera.processInput(deltaTime);

		m_DynamicLightManager.setSpotLightDirection(0, m_SceneCamera.getFront());
		m_DynamicLightManager.setSpotLightPosition(0, m_SceneCamera.getPosition());
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

	void Scene3D::addStaticModelsToRenderer() {
		auto iter = m_RenderableModels.begin();
		while (iter != m_RenderableModels.end()) {
			RenderableModel *curr = *iter;
			if (curr->getStatic()) {
				if (curr->getTransparent()) {
					m_ModelRenderer.submitTransparent(curr);
				}
				else {
					m_ModelRenderer.submitOpaque(curr);
				}
			}
			iter++;
		}
	}

	void Scene3D::addTransparentModelsToRenderer() {
		auto iter = m_RenderableModels.begin();
		while (iter != m_RenderableModels.end()) {
			RenderableModel *curr = *iter;
			if (curr->getTransparent()) {
				m_ModelRenderer.submitTransparent(curr);
			}
			iter++;
		}
	}

	void Scene3D::addTransparentStaticModelsToRenderer() {
		auto iter = m_RenderableModels.begin();
		while (iter != m_RenderableModels.end()) {
			RenderableModel *curr = *iter;
			if (curr->getStatic() && curr->getTransparent()) {
				m_ModelRenderer.submitTransparent(curr);
			}
			iter++;
		}
	}

	void Scene3D::addOpaqueModelsToRenderer() {
		auto iter = m_RenderableModels.begin();
		while (iter != m_RenderableModels.end()) {
			RenderableModel *curr = *iter;
			if (!curr->getTransparent()) {
				m_ModelRenderer.submitOpaque(curr);
			}
			iter++;
		}
	}

	void Scene3D::addOpaqueStaticModelsToRenderer() {
		auto iter = m_RenderableModels.begin();
		while (iter != m_RenderableModels.end()) {
			RenderableModel *curr = *iter;
			if (curr->getStatic() && !curr->getTransparent()) {
				m_ModelRenderer.submitOpaque(curr);
			}
			iter++;
		}
	}

}
