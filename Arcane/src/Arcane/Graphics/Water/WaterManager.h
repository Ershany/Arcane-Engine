#pragma once
#ifndef WATERMANAGER_H
#define WATERMANAGER_H

namespace Arcane
{
	class Scene;
	class WaterComponent;
	class TransformComponent;

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

		// Getters for water using reflection/refraction
		inline bool HasWaterWithRefractionReflection() const { return m_ReflectionFramebuffer != nullptr && m_RefractionFramebuffer != nullptr; }
		Framebuffer* GetWaterRefractionFramebuffer() { return m_RefractionFramebuffer; }
		Framebuffer* GetWaterReflectionFramebuffer() { return m_ReflectionFramebuffer; }
#ifdef WATER_REFLECTION_USE_MSAA
		Framebuffer* GetWaterRefractionFramebuffer() { return m_ResolveRefractionFramebuffer; }
#endif
	private:
		void FindClosestWater();

		void ReallocateReflectionTarget(Framebuffer **framebuffer, glm::uvec2 newResolution, bool multisampled);
		void ReallocateRefractionTarget(Framebuffer **framebuffer, glm::uvec2 newResolution, bool multisampled);
	private:
		Scene *m_Scene;

		// Currently only supports one reflection/refraction water surface at a time, so keep track of the closest so it can reflect and refract it so the water pass can then use these at rendering time
		WaterComponent *m_ClosestWaterComponent;
		TransformComponent *m_ClosestWaterTransform;
		int m_ClosestWaterIndex = 0;
		Framebuffer *m_ReflectionFramebuffer, *m_RefractionFramebuffer;
#ifdef WATER_REFLECTION_USE_MSAA
		Framebuffer *m_ResolveReflectionFramebuffer, *m_ResolveRefractionFramebuffer;
#endif
	};
}

#endif
