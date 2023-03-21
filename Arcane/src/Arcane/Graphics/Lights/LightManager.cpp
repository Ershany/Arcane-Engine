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
				LightBindings::BindDirectionalLight(transformComponent, lightComponent, shader, numDirLights++);
				break;
			case LightType::LightType_Point:
				LightBindings::BindPointLight(transformComponent, lightComponent, shader, numPointLights++);
				break;
			case LightType::LightType_Spot:
				LightBindings::BindSpotLight(transformComponent, lightComponent, shader, numSpotLights++);
				break;
			}
		}

		shader->SetUniform("numDirPointSpotLights", glm::ivec4(numDirLights, numPointLights, numSpotLights, 0));
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
