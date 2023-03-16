#include "arcpch.h"
#include "ForwardProbePass.h"

#include <Arcane/Scene/Scene.h>
#include <Arcane/Graphics/Mesh/Common/Cube.h>
#include <Arcane/Graphics/IBL/ProbeManager.h>
#include <Arcane/Graphics/IBL/LightProbe.h>
#include <Arcane/Graphics/IBL/ReflectionProbe.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Skybox.h>
#include <Arcane/Graphics/Renderer/Renderpass/Forward/ForwardLightingPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/ShadowmapPass.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>

namespace Arcane
{
	ForwardProbePass::ForwardProbePass(Scene *scene) : RenderPass(scene),
		m_SceneCaptureShadowFramebuffer(IBL_CAPTURE_RESOLUTION, IBL_CAPTURE_RESOLUTION, false), m_SceneCaptureLightingFramebuffer(IBL_CAPTURE_RESOLUTION, IBL_CAPTURE_RESOLUTION, false),
		m_LightProbeConvolutionFramebuffer(LIGHT_PROBE_RESOLUTION, LIGHT_PROBE_RESOLUTION, false), m_ReflectionProbeSamplingFramebuffer(REFLECTION_PROBE_RESOLUTION, REFLECTION_PROBE_RESOLUTION, false)
	{
		m_SceneCaptureSettings.TextureFormat = GL_RGBA16F;
		m_SceneCaptureCubemap.SetCubemapSettings(m_SceneCaptureSettings);

		m_SceneCaptureShadowFramebuffer.AddDepthStencilTexture(NormalizedDepthOnly).CreateFramebuffer();
		m_SceneCaptureLightingFramebuffer.AddColorTexture(FloatingPoint16).AddDepthStencilRBO(NormalizedDepthOnly).CreateFramebuffer();
		m_LightProbeConvolutionFramebuffer.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		m_ReflectionProbeSamplingFramebuffer.AddColorTexture(FloatingPoint16).CreateFramebuffer();

		for (int i = 0; i < 6; i++) {
			m_SceneCaptureCubemap.GenerateCubemapFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, IBL_CAPTURE_RESOLUTION, IBL_CAPTURE_RESOLUTION, GL_RGB, nullptr);
		}

		m_ConvolutionShader = ShaderLoader::LoadShader("LightProbe_Convolution.glsl");
		m_ImportanceSamplingShader = ShaderLoader::LoadShader("ReflectionProbe_ImportanceSampling.glsl");
	}

	ForwardProbePass::~ForwardProbePass() {}

	void ForwardProbePass::pregenerateIBL() {
		generateBRDFLUT();
		generateFallbackProbes();
	}

	void ForwardProbePass::pregenerateProbes() {
		glm::vec3 probePosition = glm::vec3(0.0f, 0.0f, 0.0f);
		generateLightProbe(probePosition);
		generateReflectionProbe(probePosition);
	}

	void ForwardProbePass::generateBRDFLUT() {
		Shader *brdfIntegrationShader = ShaderLoader::LoadShader("BRDF_Integration.glsl");
		
		// Texture settings for the BRDF LUT
		TextureSettings textureSettings;
		textureSettings.TextureFormat = GL_RG16F;
		textureSettings.IsSRGB = false;
		textureSettings.TextureWrapSMode = GL_CLAMP_TO_EDGE;
		textureSettings.TextureWrapTMode = GL_CLAMP_TO_EDGE;
		textureSettings.TextureMinificationFilterMode = GL_LINEAR;
		textureSettings.TextureMagnificationFilterMode = GL_LINEAR;
		textureSettings.TextureAnisotropyLevel = 1.0f;
		textureSettings.HasMips = false;

		Texture *brdfLUT = new Texture(textureSettings);
		brdfLUT->Generate2DTexture(BRDF_LUT_RESOLUTION, BRDF_LUT_RESOLUTION, GL_RGB);

		// Setup the framebuffer that we are using to generate our BRDF LUT
		Framebuffer brdfFramebuffer(BRDF_LUT_RESOLUTION, BRDF_LUT_RESOLUTION, false);
		brdfFramebuffer.AddColorTexture(Normalized8).CreateFramebuffer();
		brdfFramebuffer.Bind();

		// Render state
		m_GLCache->SetShader(brdfIntegrationShader);
		m_GLCache->SetDepthTest(false); // Important cause the depth buffer isn't cleared so it has zero depth

		// Render an NDC quad to the screen so we can generate the BRDF LUT
		glViewport(0, 0, BRDF_LUT_RESOLUTION, BRDF_LUT_RESOLUTION);
		brdfFramebuffer.SetColorAttachment(brdfLUT->GetTextureId(), GL_TEXTURE_2D);
		Renderer::DrawNdcPlane();
		brdfFramebuffer.SetColorAttachment(0, GL_TEXTURE_2D);

		m_GLCache->SetDepthTest(true);

		// Set the BRDF LUT for all reflection probes
		ReflectionProbe::SetBRDFLUT(brdfLUT);
	}

	void ForwardProbePass::generateFallbackProbes() {
		ProbeManager *probeManager = m_ActiveScene->GetProbeManager();
		glm::vec3 origin(0.0f, 0.0f, 0.0f);
		m_CubemapCamera.SetCenterPosition(origin);


		// Light probe generation
		LightProbe *fallbackLightProbe = new LightProbe(origin, glm::vec2(LIGHT_PROBE_RESOLUTION, LIGHT_PROBE_RESOLUTION));
		fallbackLightProbe->Generate();

		m_GLCache->SetShader(m_ConvolutionShader);
		m_GLCache->SetFaceCull(false);
		m_GLCache->SetDepthTest(false); // Important cause the depth buffer isn't cleared so it has zero depth

		m_ConvolutionShader->SetUniform("projection", m_CubemapCamera.GetProjectionMatrix());
		m_ActiveScene->GetSkybox()->GetSkyboxCubemap()->Bind(0);
		m_ConvolutionShader->SetUniform("sceneCaptureCubemap", 0);

		m_LightProbeConvolutionFramebuffer.Bind();
		glViewport(0, 0, m_LightProbeConvolutionFramebuffer.GetWidth(), m_LightProbeConvolutionFramebuffer.GetHeight());
		for (int i = 0; i < 6; i++) {
			// Setup the camera's view
			m_CubemapCamera.SwitchCameraToFace(i);
			m_ConvolutionShader->SetUniform("view", m_CubemapCamera.GetViewMatrix());

			// Convolute the scene's capture and store it in the Light Probe's cubemap
			m_LightProbeConvolutionFramebuffer.SetColorAttachment(fallbackLightProbe->GetIrradianceMap()->GetCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			Renderer::DrawNdcCube(); // Since we are sampling a cubemap, just use a cube
			m_LightProbeConvolutionFramebuffer.SetColorAttachment(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		}
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetDepthTest(true);


		// Reflection probe generation
		ReflectionProbe *fallbackReflectionProbe = new ReflectionProbe(origin, glm::vec2(REFLECTION_PROBE_RESOLUTION, REFLECTION_PROBE_RESOLUTION));
		fallbackReflectionProbe->Generate();

		// Take the capture and perform importance sampling on the cubemap's mips that represent increased roughness levels
		m_GLCache->SetShader(m_ImportanceSamplingShader);
		m_GLCache->SetFaceCull(false);
		m_GLCache->SetDepthTest(false); // Important cause the depth buffer isn't cleared so it has zero depth

		m_ImportanceSamplingShader->SetUniform("projection", m_CubemapCamera.GetProjectionMatrix());
		m_ActiveScene->GetSkybox()->GetSkyboxCubemap()->Bind(0);
		m_ImportanceSamplingShader->SetUniform("sceneCaptureCubemap", 0);

		m_ReflectionProbeSamplingFramebuffer.Bind();
		for (int mip = 0; mip < REFLECTION_PROBE_MIP_COUNT; mip++) {
			// Calculate the size of this mip and resize
			unsigned int mipWidth = m_ReflectionProbeSamplingFramebuffer.GetWidth() >> mip;
			unsigned int mipHeight = m_ReflectionProbeSamplingFramebuffer.GetHeight() >> mip;

			glViewport(0, 0, mipWidth, mipHeight);

			float mipRoughnessLevel = (float)mip / (float)(REFLECTION_PROBE_MIP_COUNT - 1);
			m_ImportanceSamplingShader->SetUniform("roughness", mipRoughnessLevel);
			for (int i = 0; i < 6; i++) {
				// Setup the camera's view
				m_CubemapCamera.SwitchCameraToFace(i);
				m_ImportanceSamplingShader->SetUniform("view", m_CubemapCamera.GetViewMatrix());

				// Importance sample the scene's capture and store it in the Reflection Probe's cubemap
				m_ReflectionProbeSamplingFramebuffer.SetColorAttachment(fallbackReflectionProbe->GetPrefilterMap()->GetCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip);
				Renderer::DrawNdcCube(); // Since we are sampling a cubemap, just use a cube
				m_ReflectionProbeSamplingFramebuffer.SetColorAttachment(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			}
		}
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetDepthTest(true);


		probeManager->SetLightProbeFallback(fallbackLightProbe);
		probeManager->SetReflectionProbeFallback(fallbackReflectionProbe);
	}

	void ForwardProbePass::generateLightProbe(glm::vec3 &probePosition) {
		LightProbe *lightProbe = new LightProbe(probePosition, glm::vec2(LIGHT_PROBE_RESOLUTION, LIGHT_PROBE_RESOLUTION));
		lightProbe->Generate();

		// Initialize step before rendering to the probe's cubemap
		m_CubemapCamera.SetCenterPosition(probePosition);
		ShadowmapPass shadowPass(m_ActiveScene, &m_SceneCaptureShadowFramebuffer);
		ForwardLightingPass lightingPass(m_ActiveScene, &m_SceneCaptureLightingFramebuffer); // Use our framebuffer when rendering

		// Render the scene to the probe's cubemap
		for (int i = 0; i < 6; i++) {
			// Setup the camera's view
			m_CubemapCamera.SwitchCameraToFace(i);

			// Shadow pass
			ShadowmapPassOutput shadowpassOutput = shadowPass.generateShadowmaps(&m_CubemapCamera, true);

			// Light pass
			m_SceneCaptureLightingFramebuffer.Bind();
			m_SceneCaptureLightingFramebuffer.SetColorAttachment(m_SceneCaptureCubemap.GetCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			LightingPassOutput output = lightingPass.ExecuteOpaqueLightingPass(shadowpassOutput, &m_CubemapCamera, true, false);
			lightingPass.ExecuteTransparentLightingPass(shadowpassOutput, output.outputFramebuffer, &m_CubemapCamera, true, false);
			m_SceneCaptureLightingFramebuffer.SetColorAttachment(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		}

		// Take the capture and apply convolution for the irradiance map (indirect diffuse liing)
		m_GLCache->SetShader(m_ConvolutionShader);
		m_GLCache->SetFaceCull(false);
		m_GLCache->SetDepthTest(false); // Important cause the depth buffer isn't cleared so it has zero depth

		m_ConvolutionShader->SetUniform("projection", m_CubemapCamera.GetProjectionMatrix());
		m_SceneCaptureCubemap.Bind(0);
		m_ConvolutionShader->SetUniform("sceneCaptureCubemap", 0);

		m_LightProbeConvolutionFramebuffer.Bind();
		glViewport(0, 0, m_LightProbeConvolutionFramebuffer.GetWidth(), m_LightProbeConvolutionFramebuffer.GetHeight());
		for (int i = 0; i < 6; i++) {
			// Setup the camera's view
			m_CubemapCamera.SwitchCameraToFace(i);
			m_ConvolutionShader->SetUniform("view", m_CubemapCamera.GetViewMatrix());

			// Convolute the scene's capture and store it in the Light Probe's cubemap
			m_LightProbeConvolutionFramebuffer.SetColorAttachment(lightProbe->GetIrradianceMap()->GetCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			Renderer::DrawNdcCube(); // Since we are sampling a cubemap, just use a cube
			m_LightProbeConvolutionFramebuffer.SetColorAttachment(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		}
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetDepthTest(true);

		ProbeManager *probeManager = m_ActiveScene->GetProbeManager();
		probeManager->AddProbe(lightProbe);
	}

	void ForwardProbePass::generateReflectionProbe(glm::vec3 &probePosition) {
		ReflectionProbe *reflectionProbe = new ReflectionProbe(probePosition, glm::vec2(REFLECTION_PROBE_RESOLUTION, REFLECTION_PROBE_RESOLUTION));
		reflectionProbe->Generate();

		// Initialize step before rendering to the probe's cubemap
		m_CubemapCamera.SetCenterPosition(probePosition);
		ShadowmapPass shadowPass(m_ActiveScene, &m_SceneCaptureShadowFramebuffer);
		ForwardLightingPass lightingPass(m_ActiveScene, &m_SceneCaptureLightingFramebuffer); // Use our framebuffer when rendering

		// Render the scene to the probe's cubemap
		for (int i = 0; i < 6; i++) {
			// Setup the camera's view
			m_CubemapCamera.SwitchCameraToFace(i);

			// Shadow pass
			ShadowmapPassOutput shadowpassOutput = shadowPass.generateShadowmaps(&m_CubemapCamera, true);

			// Light pass
			m_SceneCaptureLightingFramebuffer.Bind();
			m_SceneCaptureLightingFramebuffer.SetColorAttachment(m_SceneCaptureCubemap.GetCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			LightingPassOutput output = lightingPass.ExecuteOpaqueLightingPass(shadowpassOutput, &m_CubemapCamera, true, false);
			lightingPass.ExecuteTransparentLightingPass(shadowpassOutput, output.outputFramebuffer, &m_CubemapCamera, true, false);
			m_SceneCaptureLightingFramebuffer.SetColorAttachment(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		}

		// Take the capture and perform importance sampling on the cubemap's mips that represent increased roughness levels
		m_GLCache->SetShader(m_ImportanceSamplingShader);
		m_GLCache->SetFaceCull(false);
		m_GLCache->SetDepthTest(false); // Important cause the depth buffer isn't cleared so it has zero depth

		m_ImportanceSamplingShader->SetUniform("projection", m_CubemapCamera.GetProjectionMatrix());
		m_SceneCaptureCubemap.Bind(0);
		m_ImportanceSamplingShader->SetUniform("sceneCaptureCubemap", 0);

		m_ReflectionProbeSamplingFramebuffer.Bind();
		for (int mip = 0; mip < REFLECTION_PROBE_MIP_COUNT; mip++) {
			// Calculate the size of this mip and resize
			unsigned int mipWidth = m_ReflectionProbeSamplingFramebuffer.GetWidth() >> mip;
			unsigned int mipHeight = m_ReflectionProbeSamplingFramebuffer.GetHeight() >> mip;

			glViewport(0, 0, mipWidth, mipHeight);
			
			float mipRoughnessLevel = (float)mip / (float)(REFLECTION_PROBE_MIP_COUNT - 1);
			m_ImportanceSamplingShader->SetUniform("roughness", mipRoughnessLevel);
			for (int i = 0; i < 6; i++) {
				// Setup the camera's view
				m_CubemapCamera.SwitchCameraToFace(i);
				m_ImportanceSamplingShader->SetUniform("view", m_CubemapCamera.GetViewMatrix());

				// Importance sample the scene's capture and store it in the Reflection Probe's cubemap
				m_ReflectionProbeSamplingFramebuffer.SetColorAttachment(reflectionProbe->GetPrefilterMap()->GetCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip);
				Renderer::DrawNdcCube(); // Since we are sampling a cubemap, just use a cube
				m_ReflectionProbeSamplingFramebuffer.SetColorAttachment(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			}
		}
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetDepthTest(true);

		ProbeManager *probeManager = m_ActiveScene->GetProbeManager();
		probeManager->AddProbe(reflectionProbe);
	}
}
