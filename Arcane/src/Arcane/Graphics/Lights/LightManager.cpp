#include "arcpch.h"
#include "LightManager.h"

#include <limits>

#include <Arcane/Graphics/Lights/LightBindings.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Texture/Cubemap.h>
#include <Arcane/Scene/Components.h>
#include <Arcane/Scene/Scene.h>

namespace Arcane
{
	LightManager::LightManager(Scene *scene) : m_Scene(scene), m_DirectionalLightShadowFramebuffer(nullptr), m_SpotLightShadowFramebuffer(nullptr), m_PointLightShadowFramebuffer(nullptr), m_PointLightShadowCubemap(nullptr)
	{

	}

	LightManager::~LightManager()
	{
		delete m_DirectionalLightShadowFramebuffer;
		delete m_SpotLightShadowFramebuffer;
		delete m_PointLightShadowFramebuffer;
		delete m_PointLightShadowCubemap;
	}

	void LightManager::Init()
	{
		FindClosestDirectionalLightShadowCaster();
		FindClosestSpotLightShadowCaster();
		FindClosestPointLightShadowCaster();

		if (!m_DirectionalLightShadowFramebuffer)
		{
			m_DirectionalLightShadowFramebuffer = new Framebuffer(SHADOWMAP_RESOLUTION_X_DEFAULT, SHADOWMAP_RESOLUTION_Y_DEFAULT, false);
			m_DirectionalLightShadowFramebuffer->AddDepthStencilTexture(NormalizedDepthOnly).CreateFramebuffer();
		}
		if (!m_SpotLightShadowFramebuffer)
		{
			m_SpotLightShadowFramebuffer = new Framebuffer(SHADOWMAP_RESOLUTION_X_DEFAULT, SHADOWMAP_RESOLUTION_Y_DEFAULT, false);
			m_SpotLightShadowFramebuffer->AddDepthStencilTexture(NormalizedDepthOnly).CreateFramebuffer();
		}
		if (!m_PointLightShadowFramebuffer)
		{
			m_PointLightShadowFramebuffer = new Framebuffer(SHADOWMAP_RESOLUTION_X_DEFAULT, SHADOWMAP_RESOLUTION_Y_DEFAULT, false);
			m_PointLightShadowFramebuffer->AddDepthStencilTexture(NormalizedDepthOnly).CreateFramebuffer();
		}
	}

	
	void LightManager::Update()
	{
		// Reset out pointers since it is possible no shadow caster exists anymore
		m_ClosestDirectionalLightShadowCaster = nullptr;
		m_ClosestSpotLightShadowCaster = nullptr;
		m_ClosestPointLightShadowCaster = nullptr;
		
		FindClosestDirectionalLightShadowCaster();
		FindClosestSpotLightShadowCaster();
		FindClosestPointLightShadowCaster();
	}

	// TODO: Should use camera component's position
	void LightManager::FindClosestDirectionalLightShadowCaster()
	{
		float closestDistance2 = std::numeric_limits<float>::max();
		int currentDirectionalLightIndex = -1;

		// Prioritize the closest directional light to the camera as our directional shadow caster
		auto group = m_Scene->m_Registry.group<LightComponent>(entt::get<TransformComponent>);
		for (auto entity : group)
		{
			auto&[transformComponent, lightComponent] = group.get<TransformComponent, LightComponent>(entity);

			if (lightComponent.Type != LightType::LightType_Directional)
				continue;

			currentDirectionalLightIndex++;

			if (!lightComponent.CastShadows)
				continue;

			float currentDistance2 = glm::distance2(m_Scene->GetCamera()->GetPosition(), transformComponent.Translation);
			if (currentDistance2 < closestDistance2)
			{
				closestDistance2 = currentDistance2;
				m_ClosestDirectionalLightShadowCaster = &lightComponent;
				m_ClosestDirectionalLightShadowCasterTransform = &transformComponent;
				m_ClosestDirectionalLightIndex = currentDirectionalLightIndex;
			}
		}

		if (m_ClosestDirectionalLightShadowCaster)
		{
			// TODO:
			// Ideally we won't be re-allocating everytime we encounter a different sized shadow map. This NEEDS to be solved if we ever allow multiple directional light shadow casters
			// Just allocate the biggest and only render to a portion with glViewPort, and make sure when we sample the shadowmap we account for the smaller size as well
			glm::uvec2 requiredShadowResolution = GetShadowQualityResolution(m_ClosestDirectionalLightShadowCaster->ShadowResolution);
			if (!m_DirectionalLightShadowFramebuffer || m_DirectionalLightShadowFramebuffer->GetWidth() != requiredShadowResolution.x || m_DirectionalLightShadowFramebuffer->GetHeight() != requiredShadowResolution.y)
			{
				ReallocateTarget(&m_DirectionalLightShadowFramebuffer, requiredShadowResolution);
			}
		}
	}

	// TODO: Should use camera component's position
	void LightManager::FindClosestSpotLightShadowCaster()
	{
		float closestDistance2 = std::numeric_limits<float>::max();
		int currentSpotLightIndex = -1;

		// Prioritize the closest spot light to the camera as our spotlight shadow caster
		auto group = m_Scene->m_Registry.group<LightComponent>(entt::get<TransformComponent>);
		for (auto entity : group)
		{
			auto&[transformComponent, lightComponent] = group.get<TransformComponent, LightComponent>(entity);

			if (lightComponent.Type != LightType::LightType_Spot)
				continue;

			currentSpotLightIndex++;

			if (!lightComponent.CastShadows)
				continue;

			float currentDistance2 = glm::distance2(m_Scene->GetCamera()->GetPosition(), transformComponent.Translation);
			if (currentDistance2 < closestDistance2)
			{
				closestDistance2 = currentDistance2;
				m_ClosestSpotLightShadowCaster = &lightComponent;
				m_ClosestSpotLightShadowCasterTransform = &transformComponent;
				m_ClosestSpotLightIndex = currentSpotLightIndex;
			}
		}

		if (m_ClosestSpotLightShadowCaster)
		{
			// TODO:
			// Ideally we won't be re-allocating everytime we encounter a different sized shadow map. This NEEDS to be solved if we ever allow multiple spot light shadow casters
			// Just allocate the biggest and only render to a portion with glViewPort, and make sure when we sample the shadowmap we account for the smaller size as well
			glm::uvec2 requiredShadowResolution = GetShadowQualityResolution(m_ClosestSpotLightShadowCaster->ShadowResolution);
			if (!m_SpotLightShadowFramebuffer || m_SpotLightShadowFramebuffer->GetWidth() != requiredShadowResolution.x || m_SpotLightShadowFramebuffer->GetHeight() != requiredShadowResolution.y)
			{
				ReallocateTarget(&m_SpotLightShadowFramebuffer, requiredShadowResolution);
			}
		}
	}

	// TODO: Should use camera component's position
	void LightManager::FindClosestPointLightShadowCaster()
	{
		float closestDistance2 = std::numeric_limits<float>::max();
		int currentPointLightIndex = -1;

		// Prioritize the closest point light to the camera as our pointlight shadow caster
		auto group = m_Scene->m_Registry.group<LightComponent>(entt::get<TransformComponent>);
		for (auto entity : group)
		{
			auto& [transformComponent, lightComponent] = group.get<TransformComponent, LightComponent>(entity);

			if (lightComponent.Type != LightType::LightType_Point)
				continue;

			currentPointLightIndex++;

			if (!lightComponent.CastShadows)
				continue;

			float currentDistance2 = glm::distance2(m_Scene->GetCamera()->GetPosition(), transformComponent.Translation);
			if (currentDistance2 < closestDistance2)
			{
				closestDistance2 = currentDistance2;
				m_ClosestPointLightShadowCaster = &lightComponent;
				m_ClosestPointLightShadowCasterTranform = &transformComponent;
				m_ClosestPointLightIndex = currentPointLightIndex;
			}
		}

		if (m_ClosestPointLightShadowCaster)
		{
			// TODO:
			// Ideally we won't be re-allocating everytime we encounter a different sized shadow map. This NEEDS to be solved if we ever allow multiple point light shadow casters
			// Just allocate the biggest and only render to a portion with glViewPort, and make sure when we sample the shadowmap we account for the smaller size as well
			glm::uvec2 requiredShadowResolution = GetShadowQualityResolution(m_ClosestPointLightShadowCaster->ShadowResolution);
			if (!m_PointLightShadowFramebuffer || m_PointLightShadowFramebuffer->GetWidth() != requiredShadowResolution.x || m_PointLightShadowFramebuffer->GetHeight() != requiredShadowResolution.y)
			{
				ReallocateTarget(&m_PointLightShadowFramebuffer, requiredShadowResolution);
				ReallocateCubemap(&m_PointLightShadowCubemap, requiredShadowResolution);
			}
		}
	}

	void LightManager::ReallocateTarget(Framebuffer **framebuffer, glm::uvec2 newResolution)
	{
		if (*framebuffer) // Can this ever be garbage data or nullptr? If it is just a nullptr this isn't needed. Ehh this function is temporary until this is improved anyways
		{
			delete *framebuffer;
		}

		*framebuffer = new Framebuffer(newResolution.x, newResolution.y, false);
		(*framebuffer)->AddDepthStencilTexture(NormalizedDepthOnly).CreateFramebuffer();
	}

	void LightManager::ReallocateCubemap(Cubemap** cubemap, glm::uvec2 newResolution)
	{
		if (*cubemap) // Can this ever be garbage data or nullptr? If it is just a nullptr this isn't needed. Ehh this function is temporary until this is improved anyways
		{
			delete *cubemap;
		}

		CubemapSettings settings;
		settings.TextureFormat = GL_DEPTH_COMPONENT;
		settings.TextureMinificationFilterMode = GL_NEAREST;
		settings.TextureMagnificationFilterMode = GL_NEAREST;
		*cubemap = new Cubemap(settings);
		for (int i = 0; i < 6; i++)
		{
			(*cubemap)->GenerateCubemapFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_PointLightShadowFramebuffer->GetWidth(), m_PointLightShadowFramebuffer->GetWidth(), GL_DEPTH_COMPONENT, nullptr);
		}
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
	glm::vec3 LightManager::GetDirectionalLightShadowCasterLightDir()
	{
		if (!m_ClosestDirectionalLightShadowCaster)
		{
			ARC_ASSERT(false, "Directional shadow caster does not exist in current scene - could not get light direction");
			return glm::vec3(0.0f, -1.0f, 0.0f);
		}

		return m_ClosestDirectionalLightShadowCasterTransform->GetForward();
	}

	glm::vec2 LightManager::GetDirectionalLightShadowCasterNearFarPlane()
	{
		if (!m_ClosestDirectionalLightShadowCaster)
		{
			ARC_ASSERT(false, "Directional shadow caster does not exist in current scene - could not get near/far plane");
			return glm::vec2(SHADOWMAP_NEAR_PLANE_DEFAULT, SHADOWMAP_FAR_PLANE_DEFAULT);
		}

		return glm::vec2(m_ClosestDirectionalLightShadowCaster->ShadowNearPlane, m_ClosestDirectionalLightShadowCaster->ShadowFarPlane);
	}

	float LightManager::GetDirectionalLightShadowCasterBias()
	{
		if (!m_ClosestDirectionalLightShadowCaster)
		{
			ARC_ASSERT(false, "Directional shadow caster does not exist in current scene - could not get bias");
			return SHADOWMAP_BIAS_DEFAULT;
		}

		return m_ClosestDirectionalLightShadowCaster->ShadowBias;
	}

	int LightManager::GetDirectionalLightShadowCasterIndex()
	{
		if (!m_ClosestDirectionalLightShadowCaster)
		{
			ARC_ASSERT(false, "Directional shadow caster does not exist in current scene - could not get index");
			return -1;
		}

		return m_ClosestDirectionalLightIndex;
	}

	glm::vec3 LightManager::GetSpotLightShadowCasterLightDir()
	{
		if (!m_ClosestSpotLightShadowCaster)
		{
			ARC_ASSERT(false, "Spotlight shadow caster does not exist in current scene - could not get light direction");
			return glm::vec3(0.0f, -1.0f, 0.0f);
		}

		return m_ClosestSpotLightShadowCasterTransform->GetForward();
	}

	glm::vec3 LightManager::GetSpotLightShadowCasterLightPosition()
	{
		if (!m_ClosestSpotLightShadowCaster)
		{
			ARC_ASSERT(false, "Spotlight shadow caster does not exist in current scene - could not get light position");
			return glm::vec3(0.0f, 0.0f, 0.0f);
		}

		return m_ClosestSpotLightShadowCasterTransform->Translation;
	}

	float LightManager::GetSpotLightShadowCasterOuterCutOffAngle()
	{
		if (!m_ClosestSpotLightShadowCaster)
		{
			ARC_ASSERT(false, "Spotlight shadow caster does not exist in current scene - could not get outer cutoff angle");
			return 0.0f;
		}

		return glm::acos(m_ClosestSpotLightShadowCaster->OuterCutOff);
	}

	float LightManager::GetSpotLightShadowCasterAttenuationRange()
	{
		if (!m_ClosestSpotLightShadowCaster)
		{
			ARC_ASSERT(false, "Spotlight shadow caster does not exist in current scene - could not get attenuation range");
			return 0.0f;
		}

		return m_ClosestSpotLightShadowCaster->AttenuationRange;
	}

	glm::vec2 LightManager::GetSpotLightShadowCasterNearFarPlane()
	{
		if (!m_ClosestSpotLightShadowCaster)
		{
			ARC_ASSERT(false, "Spotlight shadow caster does not exist in current scene - could not get near/far plane");
			return glm::vec2(SHADOWMAP_NEAR_PLANE_DEFAULT, SHADOWMAP_FAR_PLANE_DEFAULT);
		}

		return glm::vec2(m_ClosestSpotLightShadowCaster->ShadowNearPlane, m_ClosestSpotLightShadowCaster->ShadowFarPlane);
	}

	float LightManager::GetSpotLightShadowCasterBias()
	{
		if (!m_ClosestSpotLightShadowCaster)
		{
			ARC_ASSERT(false, "Spotlight shadow caster does not exist in current scene - could not get bias");
			return SHADOWMAP_BIAS_DEFAULT;
		}

		return m_ClosestSpotLightShadowCaster->ShadowBias;
	}

	int LightManager::GetSpotLightShadowCasterIndex()
	{
		if (!m_ClosestSpotLightShadowCaster)
		{
			ARC_ASSERT(false, "Spotlight shadow caster does not exist in current scene - could not get index");
			return -1;
		}

		return m_ClosestSpotLightIndex;
	}

	glm::vec3 LightManager::GetPointLightShadowCasterLightPosition()
	{
		if (!m_ClosestPointLightShadowCaster)
		{
			ARC_ASSERT(false, "Point light shadow caster does not exist in current scene - could not get light position");
			return glm::vec3(0.0f, 0.0f, 0.0f);
		}

		return m_ClosestPointLightShadowCasterTranform->Translation;
	}

	glm::vec2 LightManager::GetPointLightShadowCasterNearFarPlane()
	{
		if (!m_ClosestPointLightShadowCaster)
		{
			ARC_ASSERT(false, "Point light shadow caster does not exist in current scene - could not get near/far plane");
			return glm::vec2(SHADOWMAP_NEAR_PLANE_DEFAULT, SHADOWMAP_FAR_PLANE_DEFAULT);
		}

		return glm::vec2(m_ClosestPointLightShadowCaster->ShadowNearPlane, m_ClosestPointLightShadowCaster->ShadowFarPlane);
	}

	float LightManager::GetPointLightShadowCasterBias()
	{
		if (!m_ClosestPointLightShadowCaster)
		{
			ARC_ASSERT(false, "Point light shadow caster does not exist in current scene - could not get bias");
			return SHADOWMAP_BIAS_DEFAULT;
		}

		return m_ClosestPointLightShadowCaster->ShadowBias;
	}

	int LightManager::GetPointLightShadowCasterIndex()
	{
		if (!m_ClosestPointLightShadowCaster)
		{
			ARC_ASSERT(false, "Point light shadow caster does not exist in current scene - could not get index");
			return -1;
		}

		return m_ClosestPointLightIndex;
	}
}
