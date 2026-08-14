#pragma once
#ifndef VOLUMETRICMANAGER_H
#define VOLUMETRICMANAGER_H

namespace Arcane
{
	class Scene;
	struct TransformComponent;
	struct VolumetricCloudComponent;

	enum class VolumetricNoiseGenQuality : int
	{
		VolumetricNoiseGenQuality_Low,			// 64 x 64
		VolumetricNoiseGenQuality_Medium,		// 128 x 128
		VolumetricNoiseGenQuality_High,			// 256 x 256
		VolumetricNoiseGenQuality_Ultra,		// 512 x 512
		VolumetricNoiseGenQualitySize
	};

	class VolumetricManager
	{
	public:
		VolumetricManager(Scene* scene);
		~VolumetricManager();

		void Init();
		void Update();

		TransformComponent* GetClosestVolumetricCloudTransform() { return m_ClosestVolumetricCloudTransform; }
		VolumetricCloudComponent* GetClosestVolumetricCloudComponent() { return m_ClosestVolumetricCloudComponent; }

		static glm::uvec3 GetVolumetricNoiseGenQualityResolution(VolumetricNoiseGenQuality quality);
	private:
		void FindClosestVolumetricCloud();
	private:
		Scene* m_Scene;

		// Currently only supports one volumetric cloud volume at a time, so keep track of the closest so come transparent rendering time we can use that volumetric cloud component
		VolumetricCloudComponent* m_ClosestVolumetricCloudComponent;
		TransformComponent* m_ClosestVolumetricCloudTransform;
	};
}

#endif
