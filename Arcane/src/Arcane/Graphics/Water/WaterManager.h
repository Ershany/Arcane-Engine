#pragma once
#ifndef WATERMANAGER_H
#define WATERMANAGER_H

namespace Arcane
{
	class Scene;
	struct WaterComponent;
	struct TransformComponent;
	class Framebuffer;

	enum class WaterReflectionRefractionQuality : int
	{
		WaterReflectionRefractionQuality_Low,			// 128 x 128
		WaterReflectionRefractionQuality_Medium,		// 256 x 256
		WaterReflectionRefractionQuality_High,			// 512 x 512
		WaterReflectionRefractionQuality_Ultra,			// 1024 x 1024
		WaterReflectionRefractionQuality_Nightmare,		// 2048 x 2048
		WaterReflectionRefractionQualitySize
	};

	class WaterManager
	{
	public:
		WaterManager(Scene *scene);
		~WaterManager();

		void Init();
		void Update();

		static glm::uvec2 GetWaterReflectionRefractionQualityResolution(WaterReflectionRefractionQuality quality);

		// Getters for water using reflection/refraction (right now the closest water instance is the one that gets reflection/refraction)
		inline bool HasWaterWithRefractionReflection() const { return m_ReflectionFramebuffer != nullptr && m_RefractionFramebuffer != nullptr; }
		Framebuffer* GetWaterRefractionFramebuffer() { return m_RefractionFramebuffer; }
		Framebuffer* GetWaterReflectionFramebuffer() { return m_ReflectionFramebuffer; }
#ifdef WATER_REFRACTION_USE_MSAA
		Framebuffer* GetWaterRefractionResolveFramebuffer() { return m_ResolveRefractionFramebuffer; }
#endif
#ifdef WATER_REFLECTION_USE_MSAA
		Framebuffer* GetWaterReflectionResolveFramebuffer() { return m_ResolveReflectionFramebuffer; }
#endif
		inline const WaterComponent* GetClosestWaterComponent() { return m_ClosestWaterComponent; }
		glm::vec2 GetClosestWaterReflectionNearFarPlane();
		glm::vec2 GetClosestWaterRefractionNearFarPlane();
	private:
		void FindClosestWater();

		void ReallocateReflectionTarget(Framebuffer **framebuffer, glm::uvec2 newResolution, bool multisampled);
		void ReallocateRefractionTarget(Framebuffer **framebuffer, glm::uvec2 newResolution, bool multisampled);
	private:
		Scene *m_Scene;

		// Currently only supports one reflection/refraction water surface at a time, so keep track of the closest so it can reflect and refract it so the water pass can then use these at rendering time
		WaterComponent *m_ClosestWaterComponent;
		TransformComponent *m_ClosestWaterTransform;
		Framebuffer *m_ReflectionFramebuffer, *m_RefractionFramebuffer;
#ifdef WATER_REFLECTION_USE_MSAA
		Framebuffer *m_ResolveReflectionFramebuffer, *m_ResolveRefractionFramebuffer;
#endif
	};
}

#endif
