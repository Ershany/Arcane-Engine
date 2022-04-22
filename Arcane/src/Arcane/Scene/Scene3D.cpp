#include "arcpch.h"
#include "Scene3D.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Skybox.h>
#include <Arcane/Graphics/Mesh/Mesh.h>
#include <Arcane/Graphics/Mesh/Common/Cube.h>
#include <Arcane/Graphics/Mesh/Common/Sphere.h>
#include <Arcane/Graphics/Mesh/Common/Quad.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Scene/RenderableModel.h>
#include <Arcane/Util/Loaders/TextureLoader.h>

namespace Arcane
{
	Scene3D::Scene3D(Window *window)
		: m_SceneCamera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f), m_ModelRenderer(GetCamera()), m_Terrain(glm::vec3(0.0f, -20.0f, 0.0f)), m_ProbeManager(m_SceneProbeBlendSetting)
	{
		m_GLCache = GLCache::GetInstance();

		Init();
	}

	Scene3D::~Scene3D() {
		
	}

	void Scene3D::Init() {
		TextureSettings srgbTextureSettings;
		srgbTextureSettings.IsSRGB = true;

		Model *pbrGun = new Arcane::Model("res/3D_Models/Cerberus_Gun/Cerberus_LP.FBX");
		m_RenderableModels.push_back(new RenderableModel(glm::vec3(815.0f, 150.0f, 1242.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(-90.0f), pbrGun, nullptr, true, false));

		//m_RenderableModels.push_back(new RenderableModel(glm::vec3(946.9f, 165.5f, 859.0f), glm::vec3(100.0f, 100.0f, 100.0f), glm ::vec3(1.0f, 0.0f, 0.0f), glm::radians(-90.0f), new Model(Cube()), nullptr, true, false));
		pbrGun->GetMeshes()[0].GetMaterial().SetAlbedoMap(TextureLoader::Load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_A.tga"), &srgbTextureSettings));
		pbrGun->GetMeshes()[0].GetMaterial().SetNormalMap(TextureLoader::Load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_N.tga")));
		pbrGun->GetMeshes()[0].GetMaterial().SetMetallicMap(TextureLoader::Load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_M.tga")));
		pbrGun->GetMeshes()[0].GetMaterial().SetRoughnessMap(TextureLoader::Load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_R.tga")));
		pbrGun->GetMeshes()[0].GetMaterial().SetAmbientOcclusionMap(TextureLoader::Load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_AO.tga")));

		Model *sphere = new Arcane::Model("res/3D_Models/Sphere/globe-sphere.obj");
		m_RenderableModels.push_back(new RenderableModel(glm::vec3(905.0f, 169.0f, 1131.0f), glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(-90.0f), sphere, nullptr, true, false));
		sphere->GetMeshes()[0].GetMaterial().SetAlbedoMap(TextureLoader::Load2DTexture(std::string("res/3D_Models/Sphere/rustediron2_basecolor.png"), &srgbTextureSettings));

		//Model *hyruleShield = new Arcane::Model("res/3D_Models/Hyrule_Shield/HShield.obj");
		//m_RenderableModels.push_back(new RenderableModel(glm::vec3(946.9f, 165.5f, 859.0f), glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.0f), hyruleShield, nullptr, false, false));
		//hyruleShield->getMeshes()[0].getMaterial().setAlbedoMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Hyrule_Shield/HShield_[Albedo].tga"), &srgbTextureSettings));
		//hyruleShield->getMeshes()[0].getMaterial().setNormalMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Hyrule_Shield/HShield_[Normal].tga")));
		//hyruleShield->getMeshes()[0].getMaterial().setMetallicMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Hyrule_Shield/HShield_[Metallic].tga")));
		//hyruleShield->getMeshes()[0].getMaterial().setRoughnessMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Hyrule_Shield/HShield_[Roughness].tga")));
		//hyruleShield->getMeshes()[0].getMaterial().setAmbientOcclusionMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Hyrule_Shield/HShield_[Occlusion].tga")));

		//Model *sponza = new Arcane::Model("res/3D_Models/Sponza/sponza.obj");
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

		m_SceneCamera.SetPosition(glm::vec3(90.0f, 80.0f, 180.0f));
	}

	void Scene3D::OnUpdate(float deltaTime) {
		// Camera Update
		m_SceneCamera.ProcessInput(deltaTime);

		m_DynamicLightManager.SetSpotLightDirection(0, m_SceneCamera.GetFront());
		m_DynamicLightManager.SetSpotLightPosition(0, m_SceneCamera.GetPosition());
	}

	void Scene3D::AddModelsToRenderer() {
		auto iter = m_RenderableModels.begin();
		while (iter != m_RenderableModels.end()) {
			RenderableModel *curr = *iter;
			if (curr->GetTransparent()) {
				m_ModelRenderer.SubmitTransparent(curr); 
			}
			else {
				m_ModelRenderer.SubmitOpaque(curr);
			}
			iter++;
		}
	}

	void Scene3D::AddStaticModelsToRenderer() {
		auto iter = m_RenderableModels.begin();
		while (iter != m_RenderableModels.end()) {
			RenderableModel *curr = *iter;
			if (curr->GetStatic()) {
				if (curr->GetTransparent()) {
					m_ModelRenderer.SubmitTransparent(curr);
				}
				else {
					m_ModelRenderer.SubmitOpaque(curr);
				}
			}
			iter++;
		}
	}

	void Scene3D::AddTransparentModelsToRenderer() {
		auto iter = m_RenderableModels.begin();
		while (iter != m_RenderableModels.end()) {
			RenderableModel *curr = *iter;
			if (curr->GetTransparent()) {
				m_ModelRenderer.SubmitTransparent(curr);
			}
			iter++;
		}
	}

	void Scene3D::AddTransparentStaticModelsToRenderer() {
		auto iter = m_RenderableModels.begin();
		while (iter != m_RenderableModels.end()) {
			RenderableModel *curr = *iter;
			if (curr->GetStatic() && curr->GetTransparent()) {
				m_ModelRenderer.SubmitTransparent(curr);
			}
			iter++;
		}
	}

	void Scene3D::AddOpaqueModelsToRenderer() {
		auto iter = m_RenderableModels.begin();
		while (iter != m_RenderableModels.end()) {
			RenderableModel *curr = *iter;
			if (!curr->GetTransparent()) {
				m_ModelRenderer.SubmitOpaque(curr);
			}
			iter++;
		}
	}

	void Scene3D::AddOpaqueStaticModelsToRenderer() {
		auto iter = m_RenderableModels.begin();
		while (iter != m_RenderableModels.end()) {
			RenderableModel *curr = *iter;
			if (curr->GetStatic() && !curr->GetTransparent()) {
				m_ModelRenderer.SubmitOpaque(curr);
			}
			iter++;
		}
	}
}
