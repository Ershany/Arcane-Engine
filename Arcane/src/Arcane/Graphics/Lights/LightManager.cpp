#include "arcpch.h"
#include "LightManager.h"

#include <limits>

#include <Arcane/Graphics/Lights/LightBindings.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Scene/Components.h>
#include <Arcane/Scene/Scene.h>

namespace Arcane
{
	LightManager::LightManager(Scene *scene) : m_Scene(scene), m_DirectionalShadowFramebuffer(nullptr)
	{

	}

	void LightManager::Init()
	{
		FindClosestDirectionalLightShadowCaster();
	}

	
	void LightManager::Update()
	{
		// Reset out pointers since it is possible no shadow caster exists anymore
		m_ClosestDirectionalShadowCaster = nullptr;
		
		FindClosestDirectionalLightShadowCaster();
	}

	// TODO: Should use camera component's position
	void LightManager::FindClosestDirectionalLightShadowCaster()
	{
		float closestDistance2 = std::numeric_limits<float>::max();

		// Prioritize the closest light to the camera as our directional shadow caster
		auto group = m_Scene->m_Registry.group<LightComponent>(entt::get<TransformComponent>);
		for (auto entity : group)
		{
			auto&[transformComponent, lightComponent] = group.get<TransformComponent, LightComponent>(entity);

			if (lightComponent.Type != LightType::LightType_Directional || !lightComponent.CastShadows)
				continue;

			float currentDistance2 = glm::distance2(m_Scene->GetCamera()->GetPosition(), transformComponent.Translation);
			if (currentDistance2 < closestDistance2)
			{
				m_ClosestDirectionalShadowCaster = &lightComponent;
				m_ClosestDirectionalShadowCasterTransform = &transformComponent;

				// TODO:
				// Ideally we won't be re-allocating everytime we encounter a different sized shadow map. This NEEDS to be solved if we ever allow multiple directional light shadow casters
				// Just allocate the biggest and only render to a portion with glViewPort, and make sure when we sample the shadowmap we account for the smaller size as well
				glm::uvec2 requiredShadowResolution = GetShadowQualityResolution(m_ClosestDirectionalShadowCaster->ShadowResolution);
				if (!m_DirectionalShadowFramebuffer || m_DirectionalShadowFramebuffer->GetWidth() != requiredShadowResolution.x || m_DirectionalShadowFramebuffer->GetHeight() != requiredShadowResolution.y)
				{
					ReallocateTargets(requiredShadowResolution);
				}
			}
		}
	}

	void LightManager::ReallocateTargets(glm::uvec2 newResolution)
	{
		if (m_DirectionalShadowFramebuffer)
		{
			delete m_DirectionalShadowFramebuffer;
		}

		m_DirectionalShadowFramebuffer = new Framebuffer(newResolution.x, newResolution.y, false);
		m_DirectionalShadowFramebuffer->AddDepthStencilTexture(NormalizedDepthOnly).CreateFramebuffer();
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

	glm::uvec2 LightManager::GetShadowQualityResolution(ShadowQuality quality)
	{
		switch (quality)
		{
		case ShadowQuality::ShadowQuality_Low:
			return glm::uvec2(256, 256);
			break;
		case ShadowQuality::ShadowQuality_Medium:
			return glm::uvec2(512, 512);
			break;
		case ShadowQuality::ShadowQuality_High:
			return glm::uvec2(1024, 1024);
			break;
		case ShadowQuality::ShadowQuality_Ultra:
			return glm::uvec2(2048, 2048);
			break;
		case ShadowQuality::ShadowQuality_Nightmare:
			return glm::uvec2(4096, 4096);
			break;
		default:
			ARC_ASSERT(false, "Failed to find a shadow quality resolution given the quality setting");
			return glm::uvec2(0, 0);
		}
	}

	// Getters
	glm::vec3 LightManager::GetDirectionalShadowCasterLightDir()
	{
		if (!m_ClosestDirectionalShadowCaster)
		{
			ARC_ASSERT(false, "Directional shadow caster does not exist in current scene");
			return glm::vec3(0.0f, -1.0f, 0.0f);
		}

		return m_ClosestDirectionalShadowCasterTransform->GetForward();
	}
}
