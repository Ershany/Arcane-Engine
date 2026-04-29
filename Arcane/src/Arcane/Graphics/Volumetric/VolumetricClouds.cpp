#include "arcpch.h"
#include "VolumetricClouds.h"

#include <Arcane/Graphics/Texture/Texture3D.h>
#include <Arcane/Graphics/Noise/PerlinNoise.h>
#include <Arcane/Graphics/Noise/FastNoiseLite.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Scene/Components.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>
#include <Arcane/Graphics/Camera/ICamera.h>

namespace Arcane
{
	VolumetricClouds::VolumetricClouds()
	{
		m_VolumetricCloudShader = ShaderLoader::LoadShader("Volumetric/Clouds.glsl");

		m_GLCache = GLCache::GetInstance();
	}

	void VolumetricClouds::DrawClouds(const ICamera* camera, const VolumetricCloudComponent* volumetricComponent, const TransformComponent* transformComponent)
	{
		if (volumetricComponent == nullptr || transformComponent == nullptr || volumetricComponent->GeneratedNoiseTexture3D == nullptr)
		{
			return;
		}

		m_GLCache->SetShader(m_VolumetricCloudShader);

		volumetricComponent->GeneratedNoiseTexture3D->Bind(0);
		m_VolumetricCloudShader->SetUniform("noiseTexture3D", 0);
		m_VolumetricCloudShader->SetUniform("model", transformComponent->GetTransform());
		m_VolumetricCloudShader->SetUniform("view", camera->GetViewMatrix());
		m_VolumetricCloudShader->SetUniform("projection", camera->GetProjectionMatrix());
		m_VolumetricCloudShader->SetUniform("cameraPosition", camera->GetPosition());
		m_VolumetricCloudShader->SetUniform("inverseModel", glm::inverse(camera->GetProjectionMatrix()));
		m_VolumetricCloudShader->SetUniform("scale", transformComponent->Scale);

		m_GLCache->SetDepthTest(true);
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetCullFace(GL_BACK);
		m_GLCache->SetBlend(true);


		// Note: Could do fullscreen or draw a 3D AABB representing the transform. The AABB is better for more localized volumetric effects
		//Renderer::DrawNdcPlane();
		Renderer::DrawNdcCube();

		m_GLCache->SetBlend(false);

		volumetricComponent->GeneratedNoiseTexture3D->Unbind();
	}

	Texture3D* VolumetricClouds::Generate3DNoiseTexture(NoiseTextureParams& params)
	{
		Texture3D* texture3D = new Texture3D();

		glm::uvec3 resolution = VolumetricManager::GetVolumetricNoiseGenQualityResolution(params.Quality);
		const unsigned int width = resolution.x;
		const unsigned int height = resolution.y;
		const unsigned int depth = resolution.z;

		const glm::vec3 scale = glm::vec3(params.NoiseScale.x / width, params.NoiseScale.y / height, params.NoiseScale.z / depth);
		std::vector<unsigned char> textureData(width * height * depth * 3); // 3 bytes per pixel (RGB)

		const unsigned int numThreads = std::thread::hardware_concurrency();
		const unsigned int slicesPerThread = depth / numThreads;

		std::vector<std::thread> threads;
		for (unsigned int i = 0; i < numThreads; ++i)
		{
			unsigned int zStart = i * slicesPerThread;
			unsigned int zEnd = (i == numThreads - 1) ? depth : zStart + slicesPerThread; // Last thread handles extra slices

			threads.emplace_back(Generate3DNoiseTextureChunk,
				std::ref(textureData),
				std::ref(params),
				width, height, depth,
				zStart, zEnd,
				scale);
		}

		for (auto& t : threads)
		{
			t.join();
		}

		Texture3DSettings textureSettings;
		texture3D->SetTextureSettings(textureSettings);
		texture3D->Generate3DTexture(width, height, depth, GL_RGB, GL_UNSIGNED_BYTE, textureData.data());
		return texture3D;
	}

	// Does part of the generation work so we can multi-thread it by passing threads different slices
	void VolumetricClouds::Generate3DNoiseTextureChunk(std::vector<unsigned char>& textureData, NoiseTextureParams& params, unsigned int width, unsigned int height, unsigned int depth, unsigned int zStart, unsigned int zEnd, glm::vec3& scale)
	{
		FastNoiseLite worley(static_cast<int>(params.Seed));
		worley.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
		worley.SetFrequency(1.0f);

		const siv::PerlinNoise::seed_type seed = params.Seed;
		const siv::PerlinNoise perlin{ seed };

		for (unsigned int z = zStart; z < zEnd; ++z)
		{
			for (unsigned int y = 0; y < height; ++y)
			{
				for (unsigned int x = 0; x < width; ++x)
				{
					size_t index = (z * width * height + y * width + x) * 3;

					if (params.NoiseAlgorithm == CloudNoiseAlgorithm::CloudNoiseAlgorithm_Worley)
					{
						float worleyNoise = worley.GetNoise(x * scale.x, y * scale.y, z * scale.z);
						worleyNoise = (worleyNoise + 1.0f) * 0.5f;
						worleyNoise = 1.0f - worleyNoise;
						worleyNoise = std::pow(worleyNoise, 4.0f);
						const unsigned char worleyValue = static_cast<unsigned char>(worleyNoise * 255.0f);

						textureData[index + 0] = worleyValue;
						textureData[index + 1] = worleyValue;
						textureData[index + 2] = worleyValue;
					}
					else if (params.NoiseAlgorithm == CloudNoiseAlgorithm::CloudNoiseAlgorithm_Perlin)
					{
						float perlinNoise = static_cast<float>(perlin.octave3D_01(x * scale.x, y * scale.y, z * scale.z, params.Octaves));
						const unsigned char perlinValue = static_cast<unsigned char>(perlinNoise * 255.0f);

						textureData[index + 0] = perlinValue;
						textureData[index + 1] = perlinValue;
						textureData[index + 2] = perlinValue;
					}
					else if (params.NoiseAlgorithm == CloudNoiseAlgorithm::CloudNoiseAlgorithm_WorleyPerlinMix)
					{
						float worleyNoise = worley.GetNoise(x * scale.x, y * scale.y, z * scale.z);
						worleyNoise = (worleyNoise + 1.0f) * 0.5f;
						worleyNoise = 1.0f - worleyNoise;
						worleyNoise = std::pow(worleyNoise, 8.0f);

						float perlinNoise = static_cast<float>(perlin.octave3D_01(x * scale.x, y * scale.y, z * scale.z, params.Octaves));
						float erosionStrength = 2.2f;

						float mixedNoise = worleyNoise - perlinNoise * (1 - worleyNoise) * erosionStrength;
						const unsigned char mixedValue = static_cast<unsigned char>(std::clamp(mixedNoise, 0.0f, 1.0f) * 255.0f);

						textureData[index + 0] = mixedValue;
						textureData[index + 1] = mixedValue;
						textureData[index + 2] = mixedValue;
					}
				}
			}
		}
	}
}
