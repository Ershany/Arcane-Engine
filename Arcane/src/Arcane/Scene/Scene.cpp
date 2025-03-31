#include "arcpch.h"
#include "Scene.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Skybox.h>
#include <Arcane/Graphics/Mesh/Mesh.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Scene/Entity.h>
#include <Arcane/Scene/Components.h>
#include <Arcane/Graphics/Camera/CameraController.h>
#include <Arcane/Graphics/Camera/PerspectiveCamera.h>
#include <Arcane/Graphics/Camera/OrthographicCamera.h>

namespace Arcane
{
	Scene::Scene(Window *window)
		: m_Terrain(nullptr), m_LightManager(this), m_ProbeManager(m_SceneProbeBlendSetting), m_WaterManager(this)
	{
#if USE_PERSPECTIVE_PROJ
		m_SceneCamera = new PerspectiveCamera();
#else
		m_SceneCamera = new OrthographicCamera();
#endif
		m_GLCache = GLCache::GetInstance();

		PreInit();
	}

	Scene::~Scene()
	{

	}

	void Scene::PreInit()
	{
		// Setup our ECS groupings to avoid performance costs at runtime if they get created
		auto fullOwningGroup1 = m_Registry.group<TransformComponent, MeshComponent>();
		auto partialOwningGroup1 = m_Registry.group<LightComponent>(entt::get<TransformComponent>);
		auto partialOwningGroup2 = m_Registry.group<TransformComponent, MeshComponent>(entt::get<PoseAnimatorComponent>);

		// Temp terrain things
		m_Terrain = new Terrain();
		m_Terrain->LoadTerrainFromTexture(std::string("res/terrain/heightMap.png"));
		m_Terrain->SetPosition(glm::vec3(-256.0f, -40.0f, -256.0f));

		// Skybox init needs to happen before probes are generated
		std::vector<std::string> skyboxFilePaths;
		skyboxFilePaths.push_back("res/skybox/right.png");
		skyboxFilePaths.push_back("res/skybox/left.png");
		skyboxFilePaths.push_back("res/skybox/top.png");
		skyboxFilePaths.push_back("res/skybox/bottom.png");
		skyboxFilePaths.push_back("res/skybox/back.png");
		skyboxFilePaths.push_back("res/skybox/front.png");
		m_Skybox = new Skybox(skyboxFilePaths);
	}

	void Scene::Init()
	{
		m_LightManager.Init();
		m_WaterManager.Init();
	}

	Entity Scene::CreateEntity(const std::string &name)
	{
		Entity entity = { this, m_Registry.create() };
		auto &tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Default Name" : name;
		entity.AddComponent<TransformComponent>();
		return entity;
	}

	void Scene::OnUpdate(float deltaTime)
	{
		// Camera Update
		m_SceneCamera->ProcessInput(deltaTime);

		// Update Lights
		m_LightManager.Update();

		// Update Water
		m_WaterManager.Update();

		// Update Animated Entities
		auto animatedView = m_Registry.view<PoseAnimatorComponent>();
		for (auto entity : animatedView)
		{
			auto& poseAnimator = animatedView.get<PoseAnimatorComponent>(entity);

			poseAnimator.PoseAnimator.UpdateAnimation(deltaTime);
		}
	}

	void Scene::AddModelsToRenderer(ModelFilterType filter)
	{
		auto group = m_Registry.group<TransformComponent, MeshComponent>();
		for (auto entity : group)
		{
			auto&[transform, model] = group.get<TransformComponent, MeshComponent>(entity);
			Entity currentEntity(this, entity);

			PoseAnimator *poseAnimator = nullptr;
			if (currentEntity.HasComponent<PoseAnimatorComponent>())
			{
				poseAnimator = &currentEntity.GetComponent<PoseAnimatorComponent>().PoseAnimator;
			}

			switch (filter)
			{
			case ModelFilterType::AllModels:
				Renderer::QueueMesh(model.AssetModel, transform.GetTransform(), poseAnimator, model.IsTransparent, model.ShouldBackfaceCull);
				break;
			case ModelFilterType::StaticModels:
				if (model.IsStatic)
				{
					Renderer::QueueMesh(model.AssetModel, transform.GetTransform(), poseAnimator, model.IsTransparent, model.ShouldBackfaceCull);
				}
				break;
			case ModelFilterType::OpaqueModels:
				if (model.IsTransparent == false)
				{
					Renderer::QueueMesh(model.AssetModel, transform.GetTransform(), poseAnimator, model.IsTransparent, model.ShouldBackfaceCull);
				}
				break;
			case ModelFilterType::OpaqueStaticModels:
				if (model.IsTransparent == false && model.IsStatic)
				{
					Renderer::QueueMesh(model.AssetModel, transform.GetTransform(), poseAnimator, model.IsTransparent, model.ShouldBackfaceCull);
				}
				break;
			case ModelFilterType::TransparentModels:
				if (model.IsTransparent)
				{
					Renderer::QueueMesh(model.AssetModel, transform.GetTransform(), poseAnimator, model.IsTransparent, model.ShouldBackfaceCull);
				}
				break;
			case ModelFilterType::TransparentStaticModels:
				if (model.IsTransparent && model.IsStatic)
				{
					Renderer::QueueMesh(model.AssetModel, transform.GetTransform(), poseAnimator, model.IsTransparent, model.ShouldBackfaceCull);
				}
				break;
			}
		}
	}

	ICamera* Scene::GetCamera()
	{
		return m_SceneCamera;
	}
}
