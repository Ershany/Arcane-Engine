#include "arcpch.h"
#include "WaterManager.h"

#include <Arcane/Scene/Scene.h>
#include <Arcane/Scene/Components.h>
#include <Arcane/Platform/OpenGL/Framebuffer/Framebuffer.h>

namespace Arcane
{
	WaterManager::WaterManager(Scene *scene) : m_Scene(scene), m_ReflectionFramebuffer(nullptr), m_RefractionFramebuffer(nullptr)
#ifdef WATER_REFLECTION_USE_MSAA
		, m_ResolveReflectionFramebuffer(nullptr), m_ResolveRefractionFramebuffer(nullptr)
#endif
	{

	}

	WaterManager::~WaterManager()
	{
		delete m_ReflectionFramebuffer;
		delete m_RefractionFramebuffer;
#ifdef WATER_REFLECTION_USE_MSAA
		delete m_ResolveReflectionFramebuffer;
		delete m_ResolveRefractionFramebuffer;
#endif
	}

	void WaterManager::Init()
	{
		FindClosestWater();

		// Default values if no water is found, hopefully save an allocation when we find some
#ifdef WATER_REFLECTION_USE_MSAA
		if (!m_RefractionFramebuffer)
		{
			m_RefractionFramebuffer = new Framebuffer(WATER_REFRACTION_RESOLUTION_WIDTH, WATER_REFRACTION_RESOLUTION_HEIGHT, true);
			m_RefractionFramebuffer->AddColorTexture(FloatingPoint16).AddDepthStencilTexture(NormalizedDepthOnly).CreateFramebuffer();

			m_ResolveRefractionFramebuffer = new Framebuffer(WATER_REFRACTION_RESOLUTION_WIDTH, WATER_REFRACTION_RESOLUTION_HEIGHT, false);
			m_ResolveRefractionFramebuffer->AddColorTexture(FloatingPoint16).AddDepthStencilTexture(NormalizedDepthOnly).CreateFramebuffer();
		}
		if (!m_ReflectionFramebuffer)
		{
			m_ReflectionFramebuffer = new Framebuffer(WATER_REFRACTION_RESOLUTION_WIDTH, WATER_REFRACTION_RESOLUTION_HEIGHT, true);
			m_ReflectionFramebuffer->AddColorTexture(FloatingPoint16).AddDepthStencilRBO(NormalizedDepthOnly).CreateFramebuffer();

			m_ResolveReflectionFramebuffer = new Framebuffer(WATER_REFRACTION_RESOLUTION_WIDTH, WATER_REFRACTION_RESOLUTION_HEIGHT, false);
			m_ResolveReflectionFramebuffer->AddColorTexture(FloatingPoint16).AddDepthStencilRBO(NormalizedDepthOnly).CreateFramebuffer();
		}
#else
		if (!m_RefractionFramebuffer)
		{
			m_RefractionFramebuffer = new Framebuffer(WATER_REFRACTION_RESOLUTION_WIDTH, WATER_REFRACTION_RESOLUTION_HEIGHT, false);
			m_RefractionFramebuffer->AddColorTexture(FloatingPoint16).AddDepthStencilTexture(NormalizedDepthOnly).CreateFramebuffer();
		}
		if (!m_ReflectionFramebuffer)
		{
			m_ReflectionFramebuffer = new Framebuffer(WATER_REFRACTION_RESOLUTION_WIDTH, WATER_REFRACTION_RESOLUTION_HEIGHT, false);
			m_ReflectionFramebuffer->AddColorTexture(FloatingPoint16).AddDepthStencilRBO(NormalizedDepthOnly).CreateFramebuffer();
		}
#endif
	}

	void WaterManager::Update()
	{
		// Reset our pointers since it is possible no water exists anymore
		m_ClosestWaterComponent = nullptr;

		FindClosestWater();
	}

	// TODO: Should use camera component's position
	void WaterManager::FindClosestWater()
	{
		float closestDistance2 = std::numeric_limits<float>::max();
		int currentWaterIndex = -1;

		auto group = m_Scene->m_Registry.view<TransformComponent, MeshComponent, WaterComponent>();
		for (auto entity : group)
		{
			// Water must have a mesh, otherwise nothing to render water on
			auto&[transformComponent, meshComponent, waterComponent] = group.get<TransformComponent, MeshComponent, WaterComponent>(entity);
			currentWaterIndex++;

			float currentDistance2 = glm::distance2(m_Scene->GetCamera()->GetPosition(), transformComponent.Translation);
			if (currentDistance2 < closestDistance2)
			{
				closestDistance2 = currentDistance2;
				m_ClosestWaterComponent = &waterComponent;
				m_ClosestWaterTransform = &transformComponent;
				m_ClosestWaterIndex = currentWaterIndex;
			}
		}

		if (m_ClosestWaterComponent)
		{
			// TODO:
			// Ideally we won't be re-allocating everytime we encounter a different sized reflection/refraction buffer. This NEEDS to be solved if we ever allow multiple water surfaces reflecting and refracting in a single scene
			// Just allocate the biggest and only render to a portion with glViewPort, and make sure when we sample we account for the smaller size as well

			glm::uvec2 requiredRefractionResolution = GetWaterReflectionRefractionQualityResolution(m_ClosestWaterComponent->WaterRefractionResolution);
			if (!m_RefractionFramebuffer || m_RefractionFramebuffer->GetWidth() != requiredRefractionResolution.x || m_RefractionFramebuffer->GetHeight() != requiredRefractionResolution.y)
			{
#ifdef WATER_REFLECTION_USE_MSAA
				ReallocateRefractionTarget(&m_RefractionFramebuffer, requiredRefractionResolution, true);
				ReallocateRefractionTarget(&m_ResolveRefractionFramebuffer, requiredRefractionResolution, false);
#else
				ReallocateRefractionTarget(&m_RefractionFramebuffer, requiredRefractionResolution, false);
#endif
			}

			glm::uvec2 requiredReflectionResolution = GetWaterReflectionRefractionQualityResolution(m_ClosestWaterComponent->WaterReflectionResolution);
			if (!m_ReflectionFramebuffer || m_ReflectionFramebuffer->GetWidth() != requiredReflectionResolution.x || m_ReflectionFramebuffer->GetHeight() != requiredReflectionResolution.y)
			{
#ifdef WATER_REFLECTION_USE_MSAA
				ReallocateReflectionTarget(&m_ReflectionFramebuffer, requiredReflectionResolution, true);
				ReallocateReflectionTarget(&m_ResolveReflectionFramebuffer, requiredReflectionResolution, false);
#else
				ReallocateReflectionTarget(&m_ReflectionFramebuffer, requiredReflectionResolution, false);
#endif
			}
		}
	}

	glm::uvec2 WaterManager::GetWaterReflectionRefractionQualityResolution(WaterReflectionRefractionQuality quality)
	{
		switch (quality)
		{
		case WaterReflectionRefractionQuality::WaterReflectionRefractionQuality_Low:
			return glm::uvec2(128, 128);
			break;
		case WaterReflectionRefractionQuality::WaterReflectionRefractionQuality_Medium:
			return glm::uvec2(256, 256);
			break;
		case WaterReflectionRefractionQuality::WaterReflectionRefractionQuality_High:
			return glm::uvec2(512, 512);
			break;
		case WaterReflectionRefractionQuality::WaterReflectionRefractionQuality_Ultra:
			return glm::uvec2(1024, 1024);
			break;
		case WaterReflectionRefractionQuality::WaterReflectionRefractionQuality_Nightmare:
			return glm::uvec2(2048, 2048);
			break;
		default:
			ARC_ASSERT(false, "Failed to find a water reflection/refraction quality resolution given the quality setting");
			return glm::uvec2(0, 0);
		}
	}

	void WaterManager::ReallocateReflectionTarget(Framebuffer **framebuffer, glm::uvec2 newResolution, bool multisampled)
	{
		if (*framebuffer) // TODO: Can this ever be garbage data or nullptr? If it is just a nullptr this isn't needed. Ehh this function is temporary until this is improved anyways
		{
			delete *framebuffer;
		}

		*framebuffer = new Framebuffer(newResolution.x, newResolution.y, multisampled);
		(*framebuffer)->AddColorTexture(FloatingPoint16).AddDepthStencilRBO(NormalizedDepthOnly).CreateFramebuffer();
	}

	void WaterManager::ReallocateRefractionTarget(Framebuffer **framebuffer, glm::uvec2 newResolution, bool multisampled)
	{
		if (*framebuffer) // TODO: Can this ever be garbage data or nullptr? If it is just a nullptr this isn't needed. Ehh this function is temporary until this is improved anyways
		{
			delete *framebuffer;
		}

		*framebuffer = new Framebuffer(newResolution.x, newResolution.y, multisampled);
		(*framebuffer)->AddColorTexture(FloatingPoint16).AddDepthStencilTexture(NormalizedDepthOnly).CreateFramebuffer();
	}
}
