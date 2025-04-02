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
#include <Arcane/Animation/AnimationClip.h>
#include <Arcane/Graphics/Renderer/Renderpass/MasterRenderPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/PostProcessPass.h>
#include <Arcane/Graphics/VolumetricClouds.h>


using namespace Arcane;

void Testbed::LoadTestbedDefault()
{

}

void Testbed::LoadTestbedGraphics()
{
	Scene* scene = Arcane::Application::GetInstance().GetScene();
	AssetManager& assetManager = AssetManager::GetInstance();

	// Optional post fx resources
	//Arcane::Application::GetInstance().GetMasterRenderPass()->GetPostProcessPass()->SetBloomDirtTexture(assetManager.Load2DTextureAsync(std::string("res/textures/bloom-dirt.png")));
	//Arcane::Application::GetInstance().GetMasterRenderPass()->GetPostProcessPass()->SetVignetteTexture(assetManager.Load2DTextureAsync(std::string("res/textures/vignette_mask.jpg")));

	// Load some assets for the scene at startup
	Cube* cube = new Cube();
	Quad* quad = new Quad();

	// Initialize some entities and components at startup
	{
		Model* gunModel = assetManager.LoadModelAsync(std::string("res/3D_Models/Cerberus_Gun/Cerberus_LP.FBX"),
			[](Model *loadedModel)
			{
				AssetManager& assetManager = AssetManager::GetInstance();
				auto& material = loadedModel->GetMeshes()[0].GetMaterial();

				material.SetNormalMap(assetManager.Load2DTextureAsync(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_N.tga")));
				material.SetMetallicMap(assetManager.Load2DTextureAsync(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_M.tga")));
				material.SetRoughnessMap(assetManager.Load2DTextureAsync(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_R.tga")));
				material.SetAmbientOcclusionMap(assetManager.Load2DTextureAsync(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_AO.tga")));
			}
		);

		auto gun = scene->CreateEntity("Cerberus Gun");
		auto& transformComponent = gun.GetComponent<TransformComponent>();
		transformComponent.Translation = { -32.60f, -9.28f, 48.48f };
		transformComponent.Scale = { 0.05f, 0.05f, 0.05f };
		auto& meshComponent = gun.AddComponent<MeshComponent>(gunModel);
		meshComponent.IsStatic = true;
		meshComponent.IsTransparent = false;
	}

	{
		Model* shieldModel = assetManager.LoadModelAsync(std::string("res/3D_Models/Hyrule_Shield/HShield.obj"),
			[](Model *loadedModel)
			{
				AssetManager& assetManager = AssetManager::GetInstance();
				auto& material = loadedModel->GetMeshes()[0].GetMaterial();

				TextureSettings srgbTextureSettings;
				srgbTextureSettings.IsSRGB = true;

				material.SetAlbedoMap(assetManager.Load2DTextureAsync(std::string("res/3D_Models/Hyrule_Shield/HShield_[Albedo].tga"), &srgbTextureSettings));
				material.SetNormalMap(assetManager.Load2DTextureAsync(std::string("res/3D_Models/Hyrule_Shield/HShield_[Normal].tga")));
				material.SetMetallicMap(assetManager.Load2DTextureAsync(std::string("res/3D_Models/Hyrule_Shield/HShield_[Metallic].tga")));
				material.SetRoughnessMap(assetManager.Load2DTextureAsync(std::string("res/3D_Models/Hyrule_Shield/HShield_[Roughness].tga")));
				material.SetAmbientOcclusionMap(assetManager.Load2DTextureAsync(std::string("res/3D_Models/Hyrule_Shield/HShield_[Occlusion].tga")));
			}
		);

		auto shield = scene->CreateEntity("Hyrule Shield");
		auto& transformComponent = shield.GetComponent<TransformComponent>();
		transformComponent.Translation = { -7.4f, -7.6f, -31.4f };
		auto& meshComponent = shield.AddComponent<MeshComponent>(shieldModel);
		meshComponent.IsStatic = true;
		meshComponent.IsTransparent = false;
	}

	{
		Model* cubeModel = new Model(*cube);

		auto cube = scene->CreateEntity("Cube");
		auto& transformComponent = cube.GetComponent<TransformComponent>();
		transformComponent.Scale = { 5.0f, 5.0f, 5.0f };
		auto& meshComponent = cube.AddComponent<MeshComponent>(cubeModel);
		meshComponent.IsStatic = true;
		meshComponent.IsTransparent = false;
	}

	{
		Model* quadModel = new Model(*quad);

		TextureSettings srgbTextureSettings;
		srgbTextureSettings.IsSRGB = true;

		quadModel->GetMeshes()[0].GetMaterial().SetAlbedoMap(assetManager.Load2DTextureAsync(std::string("res/textures/window.png"), &srgbTextureSettings));

		auto window = scene->CreateEntity("Window");
		auto& transformComponent = window.GetComponent<TransformComponent>();
		transformComponent.Translation = { -32.60f, 10.0f, 48.48f };
		transformComponent.Rotation = { 0.0f, glm::radians(180.0f), 0.0f };
		transformComponent.Scale = { 10.0f, 10.0f, 10.0f };
		auto& meshComponent = window.AddComponent<MeshComponent>(quadModel);
		meshComponent.IsStatic = true;
		meshComponent.IsTransparent = true;
		meshComponent.ShouldBackfaceCull = false;
	}

	{
		auto directionalLight = scene->CreateEntity("Directional Light");
		auto& transformComponent = directionalLight.GetComponent<TransformComponent>();
		transformComponent.Rotation.x = glm::radians(-120.0f);
		auto& lightComponent = directionalLight.AddComponent<LightComponent>();
		lightComponent.Type = LightType::LightType_Directional;
		lightComponent.Intensity = 2.0f;
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
		transformComponent.Rotation.x = glm::radians(-50.0f);
		auto& lightComponent = spotLight.AddComponent<LightComponent>();
		lightComponent.Type = LightType::LightType_Spot;
		lightComponent.Intensity = 150.0f;
		lightComponent.AttenuationRange = 50.0f;
		lightComponent.LightColour = glm::vec3(1.0f, 1.0f, 1.0f);
		lightComponent.IsStatic = true;
	}

	{
		auto vampire = scene->CreateEntity("Animated Vampire");
		auto& transformComponent = vampire.GetComponent<TransformComponent>();
		transformComponent.Translation = { -70.88f, -9.22f, -39.02f };
		transformComponent.Rotation.y = glm::radians(90.0f);
		transformComponent.Scale = { 0.05f, 0.05f, 0.05f };
		auto& poseAnimatorComponent = vampire.AddComponent<PoseAnimatorComponent>();

		Model* animatedVampire = assetManager.LoadModelAsync(std::string("res/3D_Models/Vampire/Dancing_Vampire.dae"),
			[&poseAnimatorComponent](Model *loadedModel)
			{
				int animIndex = 0;
				AnimationClip *clip = new AnimationClip(std::string("res/3D_Models/Vampire/Dancing_Vampire.dae"), animIndex, loadedModel);
				poseAnimatorComponent.PoseAnimator.SetAnimationClip(clip);

				AssetManager& assetManager = AssetManager::GetInstance();
				Material& meshMaterial = loadedModel->GetMeshes()[0].GetMaterial();
				meshMaterial.SetRoughnessValue(1.0f);
				meshMaterial.SetNormalMap(assetManager.Load2DTextureAsync(std::string("res/3D_Models/Vampire/textures/Vampire_normal.png")));
				meshMaterial.SetMetallicMap(assetManager.Load2DTextureAsync(std::string("res/3D_Models/Vampire/textures/Vampire_specular.png")));
			}
		);

		auto& meshComponent = vampire.AddComponent<MeshComponent>(animatedVampire);
		meshComponent.IsStatic = false;
		meshComponent.IsTransparent = false;
		meshComponent.ShouldBackfaceCull = false;
	}

	{
		auto water = scene->CreateEntity("Water");
		auto& transformComponent = water.GetComponent<TransformComponent>();
		transformComponent.Translation = { 25.0f, -14.0f, -50.0f };
		transformComponent.Rotation = { glm::radians(-90.0f), 0.0f, 0.0f };
		transformComponent.Scale = { 150.0f, 150.0f, 150.0f };
		auto& waterComponent = water.AddComponent<WaterComponent>();
		waterComponent.WaterDistortionTexture = assetManager.Load2DTextureAsync(std::string("res/water/dudv.png"));
		waterComponent.WaterNormalMap = assetManager.Load2DTextureAsync(std::string("res/water/normals.png"));
	}

	{
		auto clouds = scene->CreateEntity("Clouds");
		auto& volumetricCloudComponent = clouds.AddComponent<VolumetricCloudComponent>();
		volumetricCloudComponent.NoiseAlgorithm = CloudNoiseAlgorithm::CloudNoiseAlgorithm_Worley;
	}

	{
		Model* brickModel = new Model(*quad);

		auto bricks = scene->CreateEntity("Displaced Bricks");
		auto& transformComponent = bricks.GetComponent<TransformComponent>();
		transformComponent.Translation = { 47.70f, -6.5f, 6.0f };
		transformComponent.Rotation = { 0.0f, glm::radians(210.0f), 0.0f};
		transformComponent.Scale = { 5.0f, 5.0f, 5.0f };
		auto& meshComponent = bricks.AddComponent<MeshComponent>(brickModel);
		meshComponent.IsStatic = true;

		TextureSettings srgbTextureSettings;
		srgbTextureSettings.IsSRGB = true;

		Material& meshMaterial = meshComponent.AssetModel->GetMeshes()[0].GetMaterial();
		meshMaterial.SetAlbedoMap(assetManager.Load2DTextureAsync(std::string("res/textures/bricks2.jpg"), &srgbTextureSettings));
		meshMaterial.SetNormalMap(assetManager.Load2DTextureAsync(std::string("res/textures/bricks2_normal.jpg")));
		meshMaterial.SetDisplacementMap(assetManager.Load2DTextureAsync(std::string("res/textures/bricks2_disp.jpg")));
		meshMaterial.SetRoughnessValue(1.0f);
	}

	{
		Model* flatModel = new Model(*quad);

		auto emissionWall = scene->CreateEntity("Emission Lava");
		auto& transformComponent = emissionWall.GetComponent<TransformComponent>();
		transformComponent.Translation = { 47.70f, 6.5f, 6.0f };
		transformComponent.Rotation = { 0.0f, glm::radians(210.0f), 0.0f };
		transformComponent.Scale = { 5.0f, 5.0f, 5.0f };
		auto& meshComponent = emissionWall.AddComponent<MeshComponent>(flatModel);
		meshComponent.IsStatic = true;

		TextureSettings srgbTextureSettings;
		srgbTextureSettings.IsSRGB = true;

		Material& meshMaterial = meshComponent.AssetModel->GetMeshes()[0].GetMaterial();
		meshMaterial.SetAlbedoMap(assetManager.GetBlackSRGBTexture());
		meshMaterial.SetEmissionMap(assetManager.Load2DTextureAsync(std::string("res/textures/circuitry-emission.png"), &srgbTextureSettings));
		meshMaterial.SetEmissionIntensity(45.0f);
		meshMaterial.SetMetallicValue(1.0f);
		meshMaterial.SetRoughnessValue(1.0f);
	}

	{
		Model* cubeModel = new Model(*cube);

		auto cube = scene->CreateEntity("Emmissive Cube");
		auto& transformComponent = cube.GetComponent<TransformComponent>();
		transformComponent.Translation = { 0.0f, 15.0f, 0.0f };
		transformComponent.Scale = { 5.0f, 5.0f, 5.0f };
		auto& meshComponent = cube.AddComponent<MeshComponent>(cubeModel);
		meshComponent.IsStatic = true;
		meshComponent.IsTransparent = false;

		Material& meshMaterial = meshComponent.AssetModel->GetMeshes()[0].GetMaterial();
		meshMaterial.SetEmissionColour(glm::vec3(1.0f, 0.0f, 0.0f));
		meshMaterial.SetEmissionIntensity(15.0f);
	}

	{
		Model* brickModel = new Model(*quad);

		auto bricks = scene->CreateEntity("Emmissive Bricks");
		auto& transformComponent = bricks.GetComponent<TransformComponent>();
		transformComponent.Translation = { 47.70f, 19.5f, 6.0f };
		transformComponent.Rotation = { 0.0f, glm::radians(210.0f), 0.0f };
		transformComponent.Scale = { 5.0f, 5.0f, 5.0f };
		auto& meshComponent = bricks.AddComponent<MeshComponent>(brickModel);
		meshComponent.IsStatic = true;

		TextureSettings srgbTextureSettings;
		srgbTextureSettings.IsSRGB = true;

		Material& meshMaterial = meshComponent.AssetModel->GetMeshes()[0].GetMaterial();
		meshMaterial.SetAlbedoMap(assetManager.Load2DTextureAsync(std::string("res/textures/bricks2.jpg"), &srgbTextureSettings));
		meshMaterial.SetNormalMap(assetManager.Load2DTextureAsync(std::string("res/textures/bricks2_normal.jpg")));
		meshMaterial.SetDisplacementMap(assetManager.Load2DTextureAsync(std::string("res/textures/bricks2_disp.jpg")));
		meshMaterial.SetEmissionMap(assetManager.Load2DTextureAsync(std::string("res/textures/bricks2_emiss.png"), &srgbTextureSettings));
		meshMaterial.SetEmissionIntensity(5.0f);
		meshMaterial.SetRoughnessValue(1.0f);
	}
}

void Testbed::LoadTestbedGraphics2D()
{

}

void Testbed::LoadTestbedPhysics()
{

}

void Testbed::LoadTestbedAnimation()
{
	// TODO: This testbed should use callback API for loading models async instead of blocking to load anims (look at gfx)
	Scene* scene = Arcane::Application::GetInstance().GetScene();
	AssetManager& assetManager = AssetManager::GetInstance();

	// Load some assets for the scene at startup
	//Model* animatedVampire = assetManager.LoadModelAsync(std::string("res/3D_Models/Vampire/Dancing_Vampire.dae")); // TODO: Need an API to load textures and animation clips post async load, then I can use async for animated things
	Model* animatedVampire = assetManager.LoadModel(std::string("res/3D_Models/Vampire/Dancing_Vampire.dae"));
	int animIndex = 0;
	AnimationClip *clip = new AnimationClip(std::string("res/3D_Models/Vampire/Dancing_Vampire.dae"), animIndex, animatedVampire);

	// Initialize some entities and components at startup
	{
		auto vampire = scene->CreateEntity("Animated Vampire");
		auto& transformComponent = vampire.GetComponent<TransformComponent>();
		transformComponent.Translation = { -92.38f, -6.12f, -36.62f };
		transformComponent.Scale = { 0.05f, 0.05f, 0.05f };
		auto& meshComponent = vampire.AddComponent<MeshComponent>(animatedVampire);
		meshComponent.IsStatic = false;
		meshComponent.IsTransparent = false;
		meshComponent.ShouldBackfaceCull = false;
		Material& meshMaterial = meshComponent.AssetModel->GetMeshes()[0].GetMaterial();
		meshMaterial.SetRoughnessValue(1.0f);
		meshMaterial.SetNormalMap(assetManager.Load2DTextureAsync(std::string("res/3D_Models/Vampire/textures/Vampire_normal.png")));
		meshMaterial.SetMetallicMap(assetManager.Load2DTextureAsync(std::string("res/3D_Models/Vampire/textures/Vampire_specular.png")));
		auto& poseAnimatorComponent = vampire.AddComponent<PoseAnimatorComponent>();
		poseAnimatorComponent.PoseAnimator.SetAnimationClip(clip);
	}

	{
		auto directionalLight = scene->CreateEntity("Directional Light");
		auto& transformComponent = directionalLight.GetComponent<TransformComponent>();
		transformComponent.Rotation.x = glm::radians(-63.5f);
		auto& lightComponent = directionalLight.AddComponent<LightComponent>();
		lightComponent.Type = LightType::LightType_Directional;
		lightComponent.Intensity = 1.0f;
		lightComponent.LightColour = glm::vec3(1.0f, 0.6f, 0.0f);
		lightComponent.IsStatic = true;
		lightComponent.CastShadows = true;
		lightComponent.ShadowResolution = ShadowQuality::ShadowQuality_Ultra;
	}

	{
		auto water = scene->CreateEntity("Water");
		auto& transformComponent = water.GetComponent<TransformComponent>();
		transformComponent.Translation = { 25.0f, -14.0f, -50.0f };
		transformComponent.Rotation = { glm::radians(-90.0f), 0.0f, 0.0f };
		transformComponent.Scale = { 150.0f, 150.0f, 150.0f };
		auto& waterComponent = water.AddComponent<WaterComponent>();
		waterComponent.WaterAlbedo = glm::vec3(1.0f, 0.929f, 0.416f);
		waterComponent.AlbedoPower = 0.02f;
		waterComponent.WaterDistortionTexture = assetManager.Load2DTextureAsync(std::string("res/water/dudv.png"));
		waterComponent.WaterNormalMap = assetManager.Load2DTextureAsync(std::string("res/water/normals.png"));
	}
}
