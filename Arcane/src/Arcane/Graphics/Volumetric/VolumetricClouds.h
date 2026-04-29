#pragma once
#ifndef VOLUMETRICCLOUDS_H
#define VOLUMETRICCLOUDS_H

#ifndef VOLUMETRICMANAGER_H
#include <Arcane/Graphics/Volumetric/VolumetricManager.h>
#endif

namespace Arcane
{
	class ICamera;
	class Texture3D;
	class GLCache;
	class Shader;
	struct VolumetricCloudComponent;

	enum class CloudNoiseAlgorithm : int
	{
		CloudNoiseAlgorithm_Worley,
		CloudNoiseAlgorithm_Perlin,
		CloudNoiseAlgorithm_WorleyPerlinMix,
		CloudNoiseAlgorithmSize
	};

	struct NoiseTextureParams
	{
		CloudNoiseAlgorithm NoiseAlgorithm = CloudNoiseAlgorithm::CloudNoiseAlgorithm_Worley;
		int Octaves = 8;
		glm::vec3 NoiseScale = glm::vec3(4.0f, 4.0f, 4.0f);
		std::mt19937::result_type Seed = 12345;
		VolumetricNoiseGenQuality Quality = VolumetricNoiseGenQuality::VolumetricNoiseGenQuality_High;
	};

	class VolumetricClouds
	{
	public:
		VolumetricClouds();

		void DrawClouds(const ICamera* camera, const VolumetricCloudComponent* volumetricComponent, const TransformComponent* transformComponent);

		// Static Function for the editor to use to pre-generate a 3D noise texture that can be used for the volumetric cloud effect at runtime
		static Texture3D* Generate3DNoiseTexture(NoiseTextureParams& params);
	private:
		static void Generate3DNoiseTextureChunk(std::vector<unsigned char>& textureData, NoiseTextureParams& params, unsigned int width, unsigned int height, unsigned int depth, unsigned int zStart, unsigned int zEnd, glm::vec3& scale);
	private:
		Shader* m_VolumetricCloudShader;
		GLCache* m_GLCache;
	};
}
#endif
