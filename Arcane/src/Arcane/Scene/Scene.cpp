#include "arcpch.h"
#include "Scene.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Skybox.h>
#include <Arcane/Graphics/Mesh/Mesh.h>
#include <Arcane/Graphics/Mesh/Common/Cube.h>
#include <Arcane/Graphics/Mesh/Common/Sphere.h>
#include <Arcane/Graphics/Mesh/Common/Quad.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Scene/Entity.h>
#include <Arcane/Scene/Components.h>
#include <Arcane/Util/Loaders/AssetManager.h>

namespace Arcane
{
	Scene::Scene(Window *window)
		: m_SceneCamera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f), m_Terrain(glm::vec3(-256.0f, -40.0f, -256.0f)), m_LightManager(this), m_ProbeManager(m_SceneProbeBlendSetting)
	{
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
		m_SceneCamera.ProcessInput(deltaTime);
		m_LightManager.Update();
	}

	void Scene::AddModelsToRenderer(ModelFilterType filter)
	{
		auto group = m_Registry.group<TransformComponent, MeshComponent>();
		for (auto entity : group)
		{
			auto&[transform, model] = group.get<TransformComponent, MeshComponent>(entity);

			switch (filter)
			{
			case ModelFilterType::AllModels:
				Renderer::QueueMesh(model.AssetModel, transform.GetTransform(), model.IsTransparent);
				break;
			case ModelFilterType::StaticModels:
				if (model.IsStatic)
				{
					Renderer::QueueMesh(model.AssetModel, transform.GetTransform(), model.IsTransparent);
				}
				break;
			case ModelFilterType::OpaqueModels:
				if (model.IsTransparent == false)
				{
					Renderer::QueueMesh(model.AssetModel, transform.GetTransform(), model.IsTransparent);
				}
				break;
			case ModelFilterType::OpaqueStaticModels:
				if (model.IsTransparent == false && model.IsStatic)
				{
					Renderer::QueueMesh(model.AssetModel, transform.GetTransform(), model.IsTransparent);
				}
				break;
			case ModelFilterType::TransparentModels:
				if (model.IsTransparent)
				{
					Renderer::QueueMesh(model.AssetModel, transform.GetTransform(), model.IsTransparent);
				}
				break;
			case ModelFilterType::TransparentStaticModels:
				if (model.IsTransparent && model.IsStatic)
				{
					Renderer::QueueMesh(model.AssetModel, transform.GetTransform(), model.IsTransparent);
				}
				break;
			}
		}
	}
}
