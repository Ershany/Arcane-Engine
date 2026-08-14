#include "arcpch.h"
#include "WaterManager.h"

#include <Arcane/Scene/Scene.h>
#include <Arcane/Scene/Components.h>
#include <Arcane/Platform/OpenGL/Framebuffer/Framebuffer.h>
#include <Arcane/Graphics/Camera/ICamera.h>

namespace Arcane
{
	WaterManager::WaterManager(Scene *scene) : m_Scene(scene), m_ReflectionFramebuffer(nullptr), m_RefractionFramebuffer(nullptr), m_ClosestWaterComponent(nullptr), m_ResolveReflectionFramebuffer(nullptr), m_ResolveRefractionFramebuffer(nullptr)
	{

	}

	WaterManager::~WaterManager()
	{
		delete m_ReflectionFramebuffer;
		delete m_RefractionFramebuffer;
		delete m_ResolveReflectionFramebuffer;
		delete m_ResolveRefractionFramebuffer;
	}

	void WaterManager::Init()
	{
		FindClosestWater();
	}

	void WaterManager::Update()
	{
		FindClosestWater();
	}

	// TODO: Should use camera component's position (not the editor camera, after they are separated)
	void WaterManager::FindClosestWater()
	{
		// Reset our pointers since it is possible no water exists anymore
		m_ClosestWaterComponent = nullptr;
		float closestDistance2 = std::numeric_limits<float>::max();

		auto group = m_Scene->m_Registry.view<TransformComponent, WaterComponent>();
		for (auto entity : group)
		{
			auto&[transformComponent, waterComponent] = group.get<TransformComponent, WaterComponent>(entity);

			float currentDistance2 = glm::distance2(m_Scene->GetCamera()->GetPosition(), transformComponent.Translation);
			if (currentDistance2 < closestDistance2)
			{
				closestDistance2 = currentDistance2;
				m_ClosestWaterComponent = &waterComponent;
				m_ClosestWaterTransform = &transformComponent;
			}
		}

		if (m_ClosestWaterComponent)
		{
			// TODO:
			// Ideally we won't be re-allocating everytime we encounter a different sized reflection/refraction buffer. This NEEDS to be solved if we ever allow multiple water surfaces reflecting and refracting in a single scene
			// Just allocate the biggest and only render to a portion with glViewPort, and make sure when we sample we account for the smaller size as well
			glm::uvec2 requiredReflectionResolution = GetWaterReflectionRefractionQualityResolution(m_ClosestWaterComponent->WaterReflectionResolution);
			if (m_ClosestWaterComponent->ReflectionEnabled && (!m_ReflectionFramebuffer || m_ReflectionFramebuffer->GetWidth() != requiredReflectionResolution.x || m_ReflectionFramebuffer->GetHeight() != requiredReflectionResolution.y || (m_ReflectionFramebuffer->IsMultisampled() != m_ClosestWaterComponent->ReflectionMSAA)))
			{
				if (m_ClosestWaterComponent->ReflectionMSAA)
				{
					ReallocateReflectionTarget(&m_ReflectionFramebuffer, requiredReflectionResolution, true);
				}
				else
				{
					ReallocateReflectionTarget(&m_ReflectionFramebuffer, requiredReflectionResolution, false);
				}
			}

			glm::uvec2 requiredRefractionResolution = GetWaterReflectionRefractionQualityResolution(m_ClosestWaterComponent->WaterRefractionResolution);
			if (m_ClosestWaterComponent->ReflectionEnabled && (!m_RefractionFramebuffer || m_RefractionFramebuffer->GetWidth() != requiredRefractionResolution.x || m_RefractionFramebuffer->GetHeight() != requiredRefractionResolution.y || (m_RefractionFramebuffer->IsMultisampled() != m_ClosestWaterComponent->RefractionMSAA)))
			{
				if (m_ClosestWaterComponent->RefractionMSAA)
				{
					ReallocateRefractionTarget(&m_RefractionFramebuffer, requiredRefractionResolution, true);
				}
				else
				{
					ReallocateRefractionTarget(&m_RefractionFramebuffer, requiredRefractionResolution, false);
				}
			}

			// If we are using reflection MSAA maybe re-allocate that target if it is required
			if (m_ClosestWaterComponent->ReflectionEnabled && m_ClosestWaterComponent->ReflectionMSAA && (!m_ResolveReflectionFramebuffer || m_ResolveReflectionFramebuffer->GetWidth() != requiredReflectionResolution.x || m_ResolveReflectionFramebuffer->GetHeight() != requiredReflectionResolution.y))
			{
				ReallocateReflectionTarget(&m_ResolveReflectionFramebuffer, requiredReflectionResolution, false);
			}
			if (m_ClosestWaterComponent->RefractionEnabled && m_ClosestWaterComponent->RefractionMSAA && (!m_ResolveRefractionFramebuffer || m_ResolveRefractionFramebuffer->GetWidth() != requiredRefractionResolution.x || m_ResolveRefractionFramebuffer->GetHeight() != requiredRefractionResolution.y))
			{
				ReallocateRefractionTarget(&m_ResolveRefractionFramebuffer, requiredRefractionResolution, false);
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

	glm::vec2 WaterManager::GetClosestWaterReflectionNearFarPlane()
	{
		if (!m_ClosestWaterComponent)
		{
			ARC_ASSERT(false, "Water with Reflection does not exist in current scene - could not get near/far plane");
			return glm::vec2(WATER_REFLECTION_NEAR_PLANE_DEFAULT, WATER_REFLECTION_FAR_PLANE_DEFAULT);
		}

		return glm::vec2(m_ClosestWaterComponent->ReflectionNearPlane, m_ClosestWaterComponent->ReflectionFarPlane);
	}

	glm::vec2 WaterManager::GetClosestWaterRefractionNearFarPlane()
	{
		if (!m_ClosestWaterComponent)
		{
			ARC_ASSERT(false, "Water with Refraction does not exist in current scene - could not get near/far plane");
			return glm::vec2(WATER_REFRACTION_NEAR_PLANE_DEFAULT, WATER_REFRACTION_FAR_PLANE_DEFAULT);
		}

		return glm::vec2(m_ClosestWaterComponent->RefractionNearPlane, m_ClosestWaterComponent->RefractionFarPlane);
	}
}
