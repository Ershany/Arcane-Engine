#include "arcpch.h"
#include "VolumetricClouds.h"

#include <Arcane/Graphics/Texture/Texture3D.h>
#include <Arcane/Graphics/Noise/PerlinNoise.h>
#include <Arcane/Graphics/Noise/FastNoiseLite.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Lights/LightManager.h>
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

	void VolumetricClouds::DrawClouds(const ICamera* camera, const VolumetricCloudComponent* volumetricComponent, const TransformComponent* transformComponent, LightManager* lightManager)
	{
		if (volumetricComponent == nullptr || transformComponent == nullptr || volumetricComponent->GeneratedNoiseTexture3D == nullptr)
		{
			return;
		}

		m_GLCache->SetShader(m_VolumetricCloudShader);

		glm::mat4 modelMat = transformComponent->GetTransform();
		glm::mat4 inverseModelMat = glm::inverse(modelMat);

		// Camera position in local object space for ray origin
		glm::vec3 cameraLocalPos = glm::vec3(inverseModelMat * glm::vec4(camera->GetPosition(), 1.0f));

		// Sun direction: FROM scene TOWARD sun, in local object space
		// GetDirectionalLightShadowCasterLightDir() returns forward of the transform = direction FROM sun TOWARD scene, so we negate it
		glm::vec3 sunDirLocal = glm::vec3(0.0f, 1.0f, 0.0f); // default: straight up
		glm::vec3 sunColor    = glm::vec3(1.0f, 0.95f, 0.8f);
		if (lightManager && lightManager->HasDirectionalLightShadowCaster())
		{
			glm::vec3 lightRayDir = lightManager->GetDirectionalLightShadowCasterLightDir();
			sunDirLocal = glm::normalize(glm::vec3(inverseModelMat * glm::vec4(-lightRayDir, 0.0f)));
		}

		volumetricComponent->GeneratedNoiseTexture3D->Bind(0);
		m_VolumetricCloudShader->SetUniform("noiseTexture3D", 0);
		m_VolumetricCloudShader->SetUniform("model", modelMat);
		m_VolumetricCloudShader->SetUniform("view", camera->GetViewMatrix());
		m_VolumetricCloudShader->SetUniform("projection", camera->GetProjectionMatrix());
		m_VolumetricCloudShader->SetUniform("cameraLocalPos", cameraLocalPos);
		m_VolumetricCloudShader->SetUniform("sunDirLocal", sunDirLocal);
		m_VolumetricCloudShader->SetUniform("sunColor", sunColor);
		m_VolumetricCloudShader->SetUniform("cloudCoverage", volumetricComponent->CloudCoverage);
		m_VolumetricCloudShader->SetUniform("cloudDensity", volumetricComponent->CloudDensity);
		m_VolumetricCloudShader->SetUniform("cloudAbsorption", volumetricComponent->CloudAbsorption);
		m_VolumetricCloudShader->SetUniform("cloudAlbedo", volumetricComponent->CloudAlbedo);

		m_GLCache->SetDepthTest(true);
		// Don't write to depth for a transparent volume so objects behind still render correctly
		glDepthMask(GL_FALSE);
		// Cull front faces: back faces generate fragments as ray exit points.
		// This correctly handles both camera outside (back faces visible normally) and
		// camera inside the volume (inner back faces still visible).
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetCullFace(GL_FRONT);
		m_GLCache->SetBlend(true);
		m_GLCache->SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		Renderer::DrawNdcCube();

		m_GLCache->SetBlend(false);
		glDepthMask(GL_TRUE);
		m_GLCache->SetCullFace(GL_BACK);

		volumetricComponent->GeneratedNoiseTexture3D->Unbind();
	}

	Texture3D* VolumetricClouds::Generate3DNoiseTexture(NoiseTextureParams& params)
	{
		Texture3D* texture3D = new Texture3D();

		glm::uvec3 resolution = VolumetricManager::GetVolumetricNoiseGenQualityResolution(params.Quality);
		const unsigned int width  = resolution.x;
		const unsigned int height = resolution.y;
		const unsigned int depth  = resolution.z;

		const glm::vec3 scale = glm::vec3(params.NoiseScale.x / width, params.NoiseScale.y / height, params.NoiseScale.z / depth);
		std::vector<unsigned char> textureData(width * height * depth * 3);

		const unsigned int numThreads    = std::thread::hardware_concurrency();
		const unsigned int slicesPerThread = depth / numThreads;

		std::vector<std::thread> threads;
		for (unsigned int i = 0; i < numThreads; ++i)
		{
			unsigned int zStart = i * slicesPerThread;
			unsigned int zEnd   = (i == numThreads - 1) ? depth : zStart + slicesPerThread;

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
		textureSettings.TextureWrapSMode = GL_CLAMP_TO_EDGE;
		textureSettings.TextureWrapTMode = GL_CLAMP_TO_EDGE;
		textureSettings.TextureWrapRMode = GL_CLAMP_TO_EDGE;
		texture3D->SetTextureSettings(textureSettings);
		texture3D->Generate3DTexture(width, height, depth, GL_RGB, GL_UNSIGNED_BYTE, textureData.data());
		return texture3D;
	}

	void VolumetricClouds::Generate3DNoiseTextureChunk(std::vector<unsigned char>& textureData, NoiseTextureParams& params, unsigned int width, unsigned int height, unsigned int depth, unsigned int zStart, unsigned int zEnd, glm::vec3& scale)
	{
		// R channel: base cloud shape - Worley FBm for puffy billows
		FastNoiseLite worleyBase(static_cast<int>(params.Seed));
		worleyBase.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
		worleyBase.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
		worleyBase.SetFrequency(1.0f);
		worleyBase.SetFractalType(FastNoiseLite::FractalType_FBm);
		worleyBase.SetFractalOctaves(3);
		worleyBase.SetFractalGain(0.5f);
		worleyBase.SetFractalLacunarity(2.0f);

		// G channel: medium Worley erosion at 2x frequency
		FastNoiseLite worleyMed(static_cast<int>(params.Seed) + 1);
		worleyMed.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
		worleyMed.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
		worleyMed.SetFrequency(1.0f);

		// B channel: fine Worley erosion at 4x frequency
		FastNoiseLite worleyFine(static_cast<int>(params.Seed) + 2);
		worleyFine.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
		worleyFine.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
		worleyFine.SetFrequency(1.0f);

		const siv::PerlinNoise perlin{ static_cast<siv::PerlinNoise::seed_type>(params.Seed) };

		for (unsigned int z = zStart; z < zEnd; ++z)
		{
			for (unsigned int y = 0; y < height; ++y)
			{
				for (unsigned int x = 0; x < width; ++x)
				{
					size_t index = (z * width * height + y * width + x) * 3;

					const float sx = x * scale.x;
					const float sy = y * scale.y;
					const float sz = z * scale.z;

					// === R: Base cloud shape ===
					float rValue = 0.0f;
					if (params.NoiseAlgorithm == CloudNoiseAlgorithm::CloudNoiseAlgorithm_Worley)
					{
						float w = worleyBase.GetNoise(sx, sy, sz);
						w       = 1.0f - (w + 1.0f) * 0.5f;
						rValue  = std::pow(w, 4.0f);
					}
					else if (params.NoiseAlgorithm == CloudNoiseAlgorithm::CloudNoiseAlgorithm_Perlin)
					{
						rValue = static_cast<float>(perlin.octave3D_01(sx, sy, sz, params.Octaves));
					}
					else // WorleyPerlinMix - best for realistic billowy clouds
					{
						float w = worleyBase.GetNoise(sx, sy, sz);
						w       = 1.0f - (w + 1.0f) * 0.5f;
						float p = static_cast<float>(perlin.octave3D_01(sx, sy, sz, params.Octaves));
						rValue  = w - p * (1.0f - w) * 1.5f;
					}
					textureData[index + 0] = static_cast<unsigned char>(std::clamp(rValue, 0.0f, 1.0f) * 255.0f);

					// === G: Medium Worley erosion (2x frequency) ===
					float wMed = worleyMed.GetNoise(sx * 2.0f, sy * 2.0f, sz * 2.0f);
					wMed       = 1.0f - (wMed + 1.0f) * 0.5f;
					textureData[index + 1] = static_cast<unsigned char>(std::clamp(wMed, 0.0f, 1.0f) * 255.0f);

					// === B: Fine Worley erosion (4x frequency) ===
					float wFine = worleyFine.GetNoise(sx * 4.0f, sy * 4.0f, sz * 4.0f);
					wFine       = 1.0f - (wFine + 1.0f) * 0.5f;
					textureData[index + 2] = static_cast<unsigned char>(std::clamp(wFine, 0.0f, 1.0f) * 255.0f);
				}
			}
		}
	}
}
