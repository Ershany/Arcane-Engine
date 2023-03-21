#include "arcpch.h"
#include "LightManager.h"

#include <Arcane/Graphics/Lights/LightBindings.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Scene/Components.h>
#include <Arcane/Scene/Scene.h>

namespace Arcane
{
	LightManager::LightManager(Scene *scene) : m_Scene(scene)
	{
		Init();
	}

	void LightManager::Init()
	{
		
	}

	void LightManager::BindLightingUniforms(Shader *shader)
	{
		BindLights(shader, false);
	}

	void LightManager::BindStaticLightingUniforms(Shader *shader)
	{
		BindLights(shader, true);
	}

	void LightManager::BindLights(Shader *shader, bool bindOnlyStatic)
	{
		int numDirLights = 0, numPointLights = 0, numSpotLights = 0;

		auto group = m_Scene->m_Registry.group<LightComponent>(entt::get<TransformComponent>);
		for (auto entity : group)
		{
			auto&[transformComponent, lightComponent] = group.get<TransformComponent, LightComponent>(entity);

			if (bindOnlyStatic && !lightComponent.IsStatic)
				continue;

			switch (lightComponent.Type)
			{
			case LightType::LightType_Directional:
				ARC_ASSERT(numDirLights < LightBindings::MaxDirLights, "Directional light limit hit");
				LightBindings::BindDirectionalLight(transformComponent, lightComponent, shader, numDirLights++);
				break;
			case LightType::LightType_Point:
				ARC_ASSERT(numPointLights < LightBindings::MaxPointLights, "Point light limit hit");
				LightBindings::BindPointLight(transformComponent, lightComponent, shader, numPointLights++);
				break;
			case LightType::LightType_Spot:
				ARC_ASSERT(numSpotLights < LightBindings::MaxSpotLights, "Spot light limit hit");
				LightBindings::BindSpotLight(transformComponent, lightComponent, shader, numSpotLights++);
				break;
			}
		}

#ifdef ARC_DEV_BUILD
		numDirLights = std::min<int>(numDirLights, LightBindings::MaxDirLights);
		numPointLights = std::min<int>(numPointLights, LightBindings::MaxPointLights);
		numSpotLights = std::min<int>(numSpotLights, LightBindings::MaxSpotLights);
#endif
		shader->SetUniform("numDirPointSpotLights", glm::ivec4(numDirLights, numPointLights, numSpotLights, 0));
	}

	glm::vec2 LightManager::GetShadowQualityResolution(ShadowQuality quality)
	{
		switch (quality)
		{
		case ShadowQuality::ShadowQuality_Low:
			return glm::vec2(256, 256);
			break;
		case ShadowQuality::ShadowQuality_Medium:
			return glm::vec2(512, 512);
			break;
		case ShadowQuality::ShadowQuality_High:
			return glm::vec2(1024, 1024);
			break;
		case ShadowQuality::ShadowQuality_Ultra:
			return glm::vec2(2048, 2048);
			break;
		case ShadowQuality::ShadowQuality_Nightmare:
			return glm::vec2(4096, 4096);
			break;
		default:
			ARC_ASSERT(false, "Failed to find a shadow quality resolution given the quality setting");
			return glm::vec2(0, 0);
		}
	}

	// Getters
	glm::vec3 LightManager::GetDirectionalLightDirection(unsigned int index)
	{
		unsigned int currDirLight = 0;
		auto group = m_Scene->m_Registry.group<LightComponent>(entt::get<TransformComponent>);
		for (auto entity : group)
		{
			auto&[transformComponent, lightComponent] = group.get<TransformComponent, LightComponent>(entity);

			if (lightComponent.Type != LightType::LightType_Directional)
				continue;

			if (currDirLight++ == index)
			{
				return glm::vec3(-0.1f, -1.0f, -0.1f); // TODO: Change this to the direction using the transform's rotation
			}
		}

		ARC_ASSERT(false, "Failed to find directional light at index %u - Returning default value", index);
		return glm::vec3(-0.1f, -1.0f, -0.1f);
	}
}
