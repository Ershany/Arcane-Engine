#include "arcpch.h"
#include "VolumetricClouds.h"

#include "Arcane/Graphics/Texture/Texture3D.h"

namespace Arcane
{
	Texture3D* VolumetricClouds::Generate3DNoiseTexture(NoiseTextureParams& params)
	{
		Texture3D* texture3D = new Texture3D();

		const unsigned int width = 32;
		const unsigned int height = 32;
		const unsigned int depth = 32;

		std::vector<unsigned char> textureData(width * height * depth * 3); // 3 bytes per pixel (RGB)
		for (size_t i = 0; i < textureData.size(); i++)
		{
			if (params.NoiseAlgorithm == CloudNoiseAlgorithm::CloudNoiseAlgorithm_Worley)
			{
				textureData[i] = static_cast<unsigned char>(1.0f);
			}
			else if (params.NoiseAlgorithm == CloudNoiseAlgorithm::CloudNoiseAlgorithm_Perlin)
			{
				textureData[i] = static_cast<unsigned char>(rand() % 256);
			}
		}

		Texture3DSettings textureSettings;
		texture3D->SetTextureSettings(textureSettings);
		texture3D->Generate3DTexture(width, height, depth, GL_RGB, GL_UNSIGNED_BYTE, textureData.data());
		return texture3D;
	}
}
