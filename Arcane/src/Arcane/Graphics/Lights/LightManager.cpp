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
		// Needs to call update once in order to get the info about the shadow casters so we can generate framebuffers with proper resolution (otherwise we will need to re-allocate a framebuffer)
		Update();

		if (m_ClosestDirectionalShadowCaster)
		{
			glm::uvec2 shadowResolution = GetShadowQualityResolution(m_ClosestDirectionalShadowCaster->ShadowResolution);
			m_DirectionalShadowFramebuffer = new Framebuffer(shadowResolution.x, shadowResolution.y, false);
		}
		else
		{
			m_DirectionalShadowFramebuffer = new Framebuffer(SHADOWMAP_RESOLUTION_X, SHADOWMAP_RESOLUTION_Y, false);
		}
		m_DirectionalShadowFramebuffer->AddDepthStencilTexture(NormalizedDepthOnly).CreateFramebuffer();
	}

	// TODO: Should use camera component's position
	void LightManager::Update()
	{
		// Prioritize the closest light to the camera as our directional shadow caster (reset our pointers since it is possible no shadow caster exists anymore)
		m_ClosestDirectionalShadowCaster = nullptr;
		float closestDistance2 = 1000000.0f; // numeric max was whining about double definition so this will suffice

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
			}
		}
	}

	// TODO: Need to use pooling for this, doing a new and delete for this is just stupid
	void LightManager::ReallocateTargets()
	{
		//if (!m_DirectionalShadowFramebuffer)
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
