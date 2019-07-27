#include "pch.h"
#include "ForwardProbePass.h"

#include <graphics/mesh/common/Cube.h>
#include <graphics/ibl/ProbeManager.h>
#include <graphics/renderer/renderpass/forward/ForwardLightingPass.h>
#include <graphics/renderer/renderpass/ShadowmapPass.h>
#include <utils/loaders/ShaderLoader.h>

namespace arcane {

	ForwardProbePass::ForwardProbePass(Scene3D *scene) : RenderPass(scene),
		m_SceneCaptureShadowFramebuffer(IBL_CAPTURE_RESOLUTION, IBL_CAPTURE_RESOLUTION, false), m_SceneCaptureLightingFramebuffer(IBL_CAPTURE_RESOLUTION, IBL_CAPTURE_RESOLUTION, false),
		m_LightProbeConvolutionFramebuffer(LIGHT_PROBE_RESOLUTION, LIGHT_PROBE_RESOLUTION, false), m_ReflectionProbeSamplingFramebuffer(REFLECTION_PROBE_RESOLUTION, REFLECTION_PROBE_RESOLUTION, false)
	{
		m_SceneCaptureSettings.TextureFormat = GL_RGBA16F;
		m_SceneCaptureCubemap.setCubemapSettings(m_SceneCaptureSettings);

		m_SceneCaptureShadowFramebuffer.addDepthStencilTexture(NormalizedDepthOnly).createFramebuffer();
		m_SceneCaptureLightingFramebuffer.addColorTexture(FloatingPoint16).addDepthStencilRBO(NormalizedDepthOnly).createFramebuffer();
		m_LightProbeConvolutionFramebuffer.addColorTexture(FloatingPoint16).createFramebuffer();
		m_ReflectionProbeSamplingFramebuffer.addColorTexture(FloatingPoint16).createFramebuffer();

		for (int i = 0; i < 6; i++) {
			m_SceneCaptureCubemap.generateCubemapFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, IBL_CAPTURE_RESOLUTION, IBL_CAPTURE_RESOLUTION, GL_RGB, nullptr);
		}

		m_ConvolutionShader = ShaderLoader::loadShader("src/shaders/lightprobe_convolution.vert", "src/shaders/lightprobe_convolution.frag");
		m_ImportanceSamplingShader = ShaderLoader::loadShader("src/shaders/reflectionprobe_importance_sampling.vert", "src/shaders/reflectionprobe_importance_sampling.frag");
	}

	ForwardProbePass::~ForwardProbePass() {}

	void ForwardProbePass::pregenerateIBL() {
		generateBRDFLUT();
		generateFallbackProbes();
	}

	void ForwardProbePass::pregenerateProbes() {
		glm::vec3 probePosition = glm::vec3(67.0f, 92.0f, 133.0f);
		generateLightProbe(probePosition);
		generateReflectionProbe(probePosition);
	}

	void ForwardProbePass::generateBRDFLUT() {
		Shader *brdfIntegrationShader = ShaderLoader::loadShader("src/shaders/brdf_integration.vert", "src/shaders/brdf_integration.frag");
		ModelRenderer *modelRenderer = m_ActiveScene->getModelRenderer();
		
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
		brdfLUT->generate2DTexture(BRDF_LUT_RESOLUTION, BRDF_LUT_RESOLUTION, GL_RGB);

		// Setup the framebuffer that we are using to generate our BRDF LUT
		Framebuffer brdfFramebuffer(BRDF_LUT_RESOLUTION, BRDF_LUT_RESOLUTION, false);
		brdfFramebuffer.addColorTexture(Normalized8).createFramebuffer();
		brdfFramebuffer.bind();

		// Render state
		m_GLCache->switchShader(brdfIntegrationShader);
		m_GLCache->setDepthTest(false); // Important cause the depth buffer isn't cleared so it has zero depth

		// Render an NDC quad to the screen so we can generate the BRDF LUT
		glViewport(0, 0, BRDF_LUT_RESOLUTION, BRDF_LUT_RESOLUTION);
		brdfFramebuffer.setColorAttachment(brdfLUT->getTextureId(), GL_TEXTURE_2D);
		modelRenderer->NDC_Plane.Draw();
		brdfFramebuffer.setColorAttachment(0, GL_TEXTURE_2D);

		m_GLCache->setDepthTest(true);

		// Set the BRDF LUT for all reflection probes
		ReflectionProbe::setBRDFLUT(brdfLUT);
	}

	void ForwardProbePass::generateFallbackProbes() {
		ProbeManager *probeManager = m_ActiveScene->getProbeManager();
		glm::vec3 origin(0.0f, 0.0f, 0.0f);
		m_CubemapCamera.setCenterPosition(origin);


		// Light probe generation
		LightProbe *fallbackLightProbe = new LightProbe(origin, glm::vec2(LIGHT_PROBE_RESOLUTION, LIGHT_PROBE_RESOLUTION));
		fallbackLightProbe->generate();

		m_GLCache->switchShader(m_ConvolutionShader);
		m_GLCache->setFaceCull(false);
		m_GLCache->setDepthTest(false); // Important cause the depth buffer isn't cleared so it has zero depth

		m_ConvolutionShader->setUniform("projection", m_CubemapCamera.getProjectionMatrix());
		m_ActiveScene->getSkybox()->getSkyboxCubemap()->bind(0);
		m_ConvolutionShader->setUniform("sceneCaptureCubemap", 0);

		m_LightProbeConvolutionFramebuffer.bind();
		glViewport(0, 0, m_LightProbeConvolutionFramebuffer.getWidth(), m_LightProbeConvolutionFramebuffer.getHeight());
		for (int i = 0; i < 6; i++) {
			// Setup the camera's view
			m_CubemapCamera.switchCameraToFace(i);
			m_ConvolutionShader->setUniform("view", m_CubemapCamera.getViewMatrix());

			// Convolute the scene's capture and store it in the Light Probe's cubemap
			m_LightProbeConvolutionFramebuffer.setColorAttachment(fallbackLightProbe->getIrradianceMap()->getCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			m_ActiveScene->getModelRenderer()->NDC_Cube.Draw(); // Since we are sampling a cubemap, just use a cube
			m_LightProbeConvolutionFramebuffer.setColorAttachment(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		}
		m_GLCache->setFaceCull(true);
		m_GLCache->setDepthTest(true);


		// Reflection probe generation
		ReflectionProbe *fallbackReflectionProbe = new ReflectionProbe(origin, glm::vec2(REFLECTION_PROBE_RESOLUTION, REFLECTION_PROBE_RESOLUTION));
		fallbackReflectionProbe->generate();

		// Take the capture and perform importance sampling on the cubemap's mips that represent increased roughness levels
		m_GLCache->switchShader(m_ImportanceSamplingShader);
		m_GLCache->setFaceCull(false);
		m_GLCache->setDepthTest(false); // Important cause the depth buffer isn't cleared so it has zero depth

		m_ImportanceSamplingShader->setUniform("projection", m_CubemapCamera.getProjectionMatrix());
		m_ActiveScene->getSkybox()->getSkyboxCubemap()->bind(0);
		m_ImportanceSamplingShader->setUniform("sceneCaptureCubemap", 0);

		m_ReflectionProbeSamplingFramebuffer.bind();
		for (int mip = 0; mip < REFLECTION_PROBE_MIP_COUNT; mip++) {
			// Calculate the size of this mip and resize
			unsigned int mipWidth = m_ReflectionProbeSamplingFramebuffer.getWidth() >> mip;
			unsigned int mipHeight = m_ReflectionProbeSamplingFramebuffer.getHeight() >> mip;

			glViewport(0, 0, mipWidth, mipHeight);

			float mipRoughnessLevel = (float)mip / (float)(REFLECTION_PROBE_MIP_COUNT - 1);
			m_ImportanceSamplingShader->setUniform("roughness", mipRoughnessLevel);
			for (int i = 0; i < 6; i++) {
				// Setup the camera's view
				m_CubemapCamera.switchCameraToFace(i);
				m_ImportanceSamplingShader->setUniform("view", m_CubemapCamera.getViewMatrix());

				// Importance sample the scene's capture and store it in the Reflection Probe's cubemap
				m_ReflectionProbeSamplingFramebuffer.setColorAttachment(fallbackReflectionProbe->getPrefilterMap()->getCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip);
				m_ActiveScene->getModelRenderer()->NDC_Cube.Draw(); // Since we are sampling a cubemap, just use a cube
				m_ReflectionProbeSamplingFramebuffer.setColorAttachment(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			}
		}
		m_GLCache->setFaceCull(true);
		m_GLCache->setDepthTest(true);


		probeManager->setLightProbeFallback(fallbackLightProbe);
		probeManager->setReflectionProbeFallback(fallbackReflectionProbe);
	}

	void ForwardProbePass::generateLightProbe(glm::vec3 &probePosition) {
		LightProbe *lightProbe = new LightProbe(probePosition, glm::vec2(LIGHT_PROBE_RESOLUTION, LIGHT_PROBE_RESOLUTION));
		lightProbe->generate();

		// Initialize step before rendering to the probe's cubemap
		m_CubemapCamera.setCenterPosition(probePosition);
		ShadowmapPass shadowPass(m_ActiveScene, &m_SceneCaptureShadowFramebuffer);
		ForwardLightingPass lightingPass(m_ActiveScene, &m_SceneCaptureLightingFramebuffer);

		// Render the scene to the probe's cubemap
		for (int i = 0; i < 6; i++) {
			// Setup the camera's view
			m_CubemapCamera.switchCameraToFace(i);

			// Shadow pass
			ShadowmapPassOutput shadowpassOutput = shadowPass.generateShadowmaps(&m_CubemapCamera, true);

			// Light pass
			m_SceneCaptureLightingFramebuffer.bind();
			m_SceneCaptureLightingFramebuffer.setColorAttachment(m_SceneCaptureCubemap.getCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			lightingPass.executeLightingPass(shadowpassOutput, &m_CubemapCamera, true, false);
			m_SceneCaptureLightingFramebuffer.setColorAttachment(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		}

		// Take the capture and apply convolution for the irradiance map (indirect diffuse liing)
		m_GLCache->switchShader(m_ConvolutionShader);
		m_GLCache->setFaceCull(false);
		m_GLCache->setDepthTest(false); // Important cause the depth buffer isn't cleared so it has zero depth

		m_ConvolutionShader->setUniform("projection", m_CubemapCamera.getProjectionMatrix());
		m_SceneCaptureCubemap.bind(0);
		m_ConvolutionShader->setUniform("sceneCaptureCubemap", 0);

		m_LightProbeConvolutionFramebuffer.bind();
		glViewport(0, 0, m_LightProbeConvolutionFramebuffer.getWidth(), m_LightProbeConvolutionFramebuffer.getHeight());
		for (int i = 0; i < 6; i++) {
			// Setup the camera's view
			m_CubemapCamera.switchCameraToFace(i);
			m_ConvolutionShader->setUniform("view", m_CubemapCamera.getViewMatrix());

			// Convolute the scene's capture and store it in the Light Probe's cubemap
			m_LightProbeConvolutionFramebuffer.setColorAttachment(lightProbe->getIrradianceMap()->getCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			m_ActiveScene->getModelRenderer()->NDC_Cube.Draw(); // Since we are sampling a cubemap, just use a cube
			m_LightProbeConvolutionFramebuffer.setColorAttachment(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		}
		m_GLCache->setFaceCull(true);
		m_GLCache->setDepthTest(true);

		ProbeManager *probeManager = m_ActiveScene->getProbeManager();
		probeManager->addProbe(lightProbe);
	}

	void ForwardProbePass::generateReflectionProbe(glm::vec3 &probePosition) {
		ReflectionProbe *reflectionProbe = new ReflectionProbe(probePosition, glm::vec2(REFLECTION_PROBE_RESOLUTION, REFLECTION_PROBE_RESOLUTION));
		reflectionProbe->generate();

		// Initialize step before rendering to the probe's cubemap
		m_CubemapCamera.setCenterPosition(probePosition);
		ShadowmapPass shadowPass(m_ActiveScene, &m_SceneCaptureShadowFramebuffer);
		ForwardLightingPass lightingPass(m_ActiveScene, &m_SceneCaptureLightingFramebuffer);

		// Render the scene to the probe's cubemap
		for (int i = 0; i < 6; i++) {
			// Setup the camera's view
			m_CubemapCamera.switchCameraToFace(i);

			// Shadow pass
			ShadowmapPassOutput shadowpassOutput = shadowPass.generateShadowmaps(&m_CubemapCamera, true);

			// Light pass
			m_SceneCaptureLightingFramebuffer.bind();
			m_SceneCaptureLightingFramebuffer.setColorAttachment(m_SceneCaptureCubemap.getCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			lightingPass.executeLightingPass(shadowpassOutput, &m_CubemapCamera, true, false);
			m_SceneCaptureLightingFramebuffer.setColorAttachment(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		}

		// Take the capture and perform importance sampling on the cubemap's mips that represent increased roughness levels
		m_GLCache->switchShader(m_ImportanceSamplingShader);
		m_GLCache->setFaceCull(false);
		m_GLCache->setDepthTest(false); // Important cause the depth buffer isn't cleared so it has zero depth

		m_ImportanceSamplingShader->setUniform("projection", m_CubemapCamera.getProjectionMatrix());
		m_SceneCaptureCubemap.bind(0);
		m_ImportanceSamplingShader->setUniform("sceneCaptureCubemap", 0);

		m_ReflectionProbeSamplingFramebuffer.bind();
		for (int mip = 0; mip < REFLECTION_PROBE_MIP_COUNT; mip++) {
			// Calculate the size of this mip and resize
			unsigned int mipWidth = m_ReflectionProbeSamplingFramebuffer.getWidth() >> mip;
			unsigned int mipHeight = m_ReflectionProbeSamplingFramebuffer.getHeight() >> mip;

			glViewport(0, 0, mipWidth, mipHeight);
			
			float mipRoughnessLevel = (float)mip / (float)(REFLECTION_PROBE_MIP_COUNT - 1);
			m_ImportanceSamplingShader->setUniform("roughness", mipRoughnessLevel);
			for (int i = 0; i < 6; i++) {
				// Setup the camera's view
				m_CubemapCamera.switchCameraToFace(i);
				m_ImportanceSamplingShader->setUniform("view", m_CubemapCamera.getViewMatrix());

				// Importance sample the scene's capture and store it in the Reflection Probe's cubemap
				m_ReflectionProbeSamplingFramebuffer.setColorAttachment(reflectionProbe->getPrefilterMap()->getCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip);
				m_ActiveScene->getModelRenderer()->NDC_Cube.Draw(); // Since we are sampling a cubemap, just use a cube
				m_ReflectionProbeSamplingFramebuffer.setColorAttachment(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			}
		}
		m_GLCache->setFaceCull(true);
		m_GLCache->setDepthTest(true);

		ProbeManager *probeManager = m_ActiveScene->getProbeManager();
		probeManager->addProbe(reflectionProbe);
	}

}
