#include "arcpch.h"
#include "Testbed.h"

#include <Arcane/Core/Application.h>
#include <Arcane/Util/Loaders/AssetManager.h>
#include <Arcane/Graphics/Mesh/Common/Cube.h>
#include <Arcane/Graphics/Mesh/Common/Quad.h>
#include <Arcane/Graphics/Mesh/Model.h>
#include <Arcane/Scene/Components.h>
#include <Arcane/Scene/Scene.h>
#include <Arcane/Scene/Entity.h>


using namespace Arcane;

void Testbed::LoadTestbedDefault()
{

}

void Testbed::LoadTestbedGraphics()
{
	Scene* scene = Arcane::Application::GetInstance().GetScene();
	AssetManager& assetManager = AssetManager::GetInstance();

	// Load some assets for the scene at startup
	Model* gunModel = assetManager.LoadModelAsync(std::string("res/3D_Models/Cerberus_Gun/Cerberus_LP.FBX"));
	Model* shieldModel = assetManager.LoadModelAsync(std::string("res/3D_Models/Hyrule_Shield/HShield.obj"));
	Cube* cube = new Cube();
	Model* cubeModel = new Model(*cube);
	Quad* quad = new Quad();
	Model* quadModel = new Model(*quad);
	quadModel->GetMeshes()[0].GetMaterial().SetAlbedoMap(assetManager.Load2DTextureAsync(std::string("res/textures/window.png")));

	// Initialize some entities and components at startup
	{
		auto gun = scene->CreateEntity("Cerberus Gun");
		auto& transformComponent = gun.GetComponent<TransformComponent>();
		transformComponent.Translation = { -32.60f, -9.28f, 48.48f };
		transformComponent.Scale = { 0.05f, 0.05f, 0.05f };
		auto& meshComponent = gun.AddComponent<MeshComponent>(gunModel);
		meshComponent.IsStatic = true;
		meshComponent.IsTransparent = false;
	}

	{
		auto shield = scene->CreateEntity("Hyrule Shield");
		auto& transformComponent = shield.GetComponent<TransformComponent>();
		transformComponent.Translation = { -7.4f, -7.6f, -31.4f };
		auto& meshComponent = shield.AddComponent<MeshComponent>(shieldModel);
		meshComponent.IsStatic = true;
		meshComponent.IsTransparent = false;
	}

	{
		auto cube = scene->CreateEntity("Cube");
		auto& transformComponent = cube.GetComponent<TransformComponent>();
		transformComponent.Scale = { 5.0f, 5.0f, 5.0f };
		auto& meshComponent = cube.AddComponent<MeshComponent>(cubeModel);
		meshComponent.IsStatic = true;
		meshComponent.IsTransparent = false;
	}

	{
		auto window = scene->CreateEntity("Window");
		auto& transformComponent = window.GetComponent<TransformComponent>();
		transformComponent.Translation = { -32.60f, 10.0f, 48.48f };
		glm::vec3 rotation = { 0.0f, glm::radians(180.0f), 0.0f };
		transformComponent.Rotation = glm::quat(rotation);
		transformComponent.Scale = { 10.0f, 10.0f, 10.0f };
		auto& meshComponent = window.AddComponent<MeshComponent>(quadModel);
		meshComponent.IsStatic = true;
		meshComponent.IsTransparent = true;
	}

	{
		auto directionalLight = scene->CreateEntity("Directional Light");
		auto& transformComponent = directionalLight.GetComponent<TransformComponent>();
		glm::vec3 rotation = { glm::radians(-120.0f), 0.0f, 0.0f };
		transformComponent.Rotation = glm::quat(rotation);
		auto& lightComponent = directionalLight.AddComponent<LightComponent>();
		lightComponent.Type = LightType::LightType_Directional;
		lightComponent.Intensity = 3.0f;
		lightComponent.LightColour = glm::vec3(1.0f, 1.0f, 1.0f);
		lightComponent.IsStatic = true;
		lightComponent.CastShadows = true;
		lightComponent.ShadowResolution = ShadowQuality::ShadowQuality_Ultra;
	}

	{
		auto pointLight = scene->CreateEntity("Point Light1");
		auto& transformComponent = pointLight.GetComponent<TransformComponent>();
		transformComponent.Translation = glm::vec3(24.1f, 2.2f, 47.5f);
		auto& lightComponent = pointLight.AddComponent<LightComponent>();
		lightComponent.Type = LightType::LightType_Point;
		lightComponent.Intensity = 10.0f;
		lightComponent.LightColour = glm::vec3(0.0f, 1.0f, 0.0f);
		lightComponent.AttenuationRange = 30.0f;
		lightComponent.IsStatic = false;
	}

	{
		auto pointLight = scene->CreateEntity("Point Light2");
		auto& transformComponent = pointLight.GetComponent<TransformComponent>();
		transformComponent.Translation = glm::vec3(-27.2f, -9.0f, 52.0f);
		auto& lightComponent = pointLight.AddComponent<LightComponent>();
		lightComponent.Type = LightType::LightType_Point;
		lightComponent.Intensity = 30.0f;
		lightComponent.LightColour = glm::vec3(1.0f, 0.0f, 1.0f);
		lightComponent.AttenuationRange = 30.0f;
		lightComponent.IsStatic = true;
	}

	{
		auto spotLight = scene->CreateEntity("Spot Light1");
		auto& transformComponent = spotLight.GetComponent<TransformComponent>();
		transformComponent.Translation = glm::vec3(-86.9f, -5.0f, -28.2f);
		glm::vec3 rotation = { glm::radians(-50.0f), 0.0f, 0.0f };
		transformComponent.Rotation = glm::quat(rotation);
		auto& lightComponent = spotLight.AddComponent<LightComponent>();
		lightComponent.Type = LightType::LightType_Spot;
		lightComponent.Intensity = 150.0f;
		lightComponent.AttenuationRange = 50.0f;
		lightComponent.LightColour = glm::vec3(1.0f, 1.0f, 1.0f);
		lightComponent.IsStatic = true;
	}
}

void Testbed::LoadTestbedPhysics()
{
	/*
	{
		Cube* cube = new Cube();
		Model* boxModel = new Model(*cube);
		box = m_EditorScene->CreateEntity("box");
		Arcane::TransformComponent& transformComponent = box.GetComponent<TransformComponent>();
		transformComponent.Translation = { 0.f, 10.f, -10.f };
		//transformComponent.Rotation = { 0.0f, glm::radians(180.0f), 0.0f };
		//transformComponent.Scale = { 10.0f, 10.0f, 10.0f };

		// Create a test object and move it around 
		phGeometry* box1 = PhysicsFactory::CreateBoxGeometry(0.5f, 0.5f, 0.5f);
		manager->AddRigidbodyComponent(transformComponent, box1, true);

		auto& meshComponent = box.AddComponent<MeshComponent>(boxModel);
		meshComponent.IsStatic = false;
		meshComponent.IsTransparent = false;
	}
	*/
}
