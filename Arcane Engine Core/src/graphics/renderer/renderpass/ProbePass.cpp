#include "pch.h"
#include "ProbePass.h"

#include <graphics/mesh/common/Cube.h>
#include <graphics/ibl/ProbeManager.h>
#include <graphics/renderer/renderpass/LightingPass.h>
#include <graphics/renderer/renderpass/ShadowmapPass.h>
#include <utils/loaders/ShaderLoader.h>

namespace arcane {

	ProbePass::ProbePass(Scene3D *scene) : RenderPass(scene, RenderPassType::ProbePassType),
		m_SceneCaptureShadowFramebuffer(IBL_CAPTURE_RESOLUTION, IBL_CAPTURE_RESOLUTION), m_SceneCaptureLightingFramebuffer(IBL_CAPTURE_RESOLUTION, IBL_CAPTURE_RESOLUTION),
		m_LightProbeConvolutionFramebuffer(LIGHT_PROBE_RESOLUTION, LIGHT_PROBE_RESOLUTION), m_SceneCaptureCubemap(m_SceneCaptureSettings)
	{
		m_SceneCaptureShadowFramebuffer.addDepthAttachment(false).createFramebuffer();
		m_SceneCaptureLightingFramebuffer.addTexture2DColorAttachment(false).addDepthStencilRBO(false).createFramebuffer();
		m_LightProbeConvolutionFramebuffer.addTexture2DColorAttachment(false).addDepthStencilRBO(false).createFramebuffer();

		for (int i = 0; i < 6; i++) {
			m_SceneCaptureCubemap.generateCubemapFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, IBL_CAPTURE_RESOLUTION, IBL_CAPTURE_RESOLUTION, GL_RGBA16F, GL_RGB, nullptr);
		}

		m_ConvolutionShader = ShaderLoader::loadShader("src/shaders/lightprobe_convolution.vert", "src/shaders/lightprobe_convolution.frag");
	}

	ProbePass::~ProbePass() {}

	void ProbePass::pregenerateProbes() {
		glm::vec3 probePosition = glm::vec3(67.0f, 92.0f, 133.0f);
		generateLightProbe(probePosition);
		/*
		// Generate the cubemap for the probe
		glm::vec3 probePosition = glm::vec3(67.0f, 92.0f, 133.0f);
		LightProbe *lightProbe = new LightProbe(probePosition, glm::vec2(IBL_CAPTURE_RESOLUTION, IBL_CAPTURE_RESOLUTION));
		lightProbe->generate();

		// Initialize step before rendering to the probe's cubemap
		m_CubemapCamera.setCenterPosition(probePosition);
		ShadowmapPass shadowPass(m_ActiveScene, &m_SceneCaptureShadowFramebuffer);
		LightingPass lightingPass(m_ActiveScene, &m_SceneCaptureLightingFramebuffer, false);

		// Render the environment to the probe's cubemap
		for (int i = 0; i < 6; i++) {
			// Setup the camera's view
			m_CubemapCamera.switchCameraToFace(i);

			// Shadow pass
			ShadowmapPassOutput shadowpassOutput = shadowPass.generateShadowmaps(&m_CubemapCamera);

			// Light pass
			m_SceneCaptureLightingFramebuffer.bind();
			m_SceneCaptureLightingFramebuffer.setColorAttachment(lightProbe->getIrradianceMap()->getCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			lightingPass.executeRenderPass(shadowpassOutput, &m_CubemapCamera);
			m_SceneCaptureLightingFramebuffer.setColorAttachment(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		}

		// Take the capture and apply convolution for the irradiance map (indirect diffuse lighting)
		// Since we are rendering to and from a cubemap, use a cube for this process
		Model cubeModel = Model(Cube());
		RenderableModel renderable(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, &cubeModel, nullptr);
		m_GLCache->switchShader(m_ConvolutionShader);
		m_ConvolutionShader->setUniformMat4("projection", m_CubemapCamera.getProjectionMatrix());
		m_LightProbeConvolutionFramebuffer.bind();
		for (int i = 0; i < 6; i++) {
			// Setup the camera's view
			m_CubemapCamera.switchCameraToFace(i);
			m_ConvolutionShader->setUniformMat4("view", m_CubemapCamera.getViewMatrix());

			// Set the probe's irradiance cubemap face as the render target on the framebuffer
			//m_LightProbeConvolutionFramebuffer.setColorAttachment(iblProbe->getIrradianceMap()->getCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			//todo: send the samplerCube to the shader

			// Finally convolute the captured scene
			//renderable.draw(m_ConvolutionShader, RenderPassType::ShadowmapPassType);
		}

		ProbeManager *probeManager = m_ActiveScene->getProbeManager();
		probeManager->addProbe(lightProbe);
		*/
	}

	void ProbePass::generateLightProbe(glm::vec3 &probePosition) {
		LightProbe* lightProbe = new LightProbe(probePosition, glm::vec2(LIGHT_PROBE_RESOLUTION, LIGHT_PROBE_RESOLUTION));
		lightProbe->generate();

		// Initialize step before rendering to the probe's cubemap
		m_CubemapCamera.setCenterPosition(probePosition);
		ShadowmapPass shadowPass(m_ActiveScene, &m_SceneCaptureShadowFramebuffer);
		LightingPass lightingPass(m_ActiveScene, &m_SceneCaptureLightingFramebuffer, false);

		// Render the scene to the probe's cubemap
		for (int i = 0; i < 6; i++) {
			// Setup the camera's view
			m_CubemapCamera.switchCameraToFace(i);

			// Shadow pass
			ShadowmapPassOutput shadowpassOutput = shadowPass.generateShadowmaps(&m_CubemapCamera);

			// Light pass
			m_SceneCaptureLightingFramebuffer.bind();
			m_SceneCaptureLightingFramebuffer.setColorAttachment(m_SceneCaptureCubemap.getCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			lightingPass.executeRenderPass(shadowpassOutput, &m_CubemapCamera);
			m_SceneCaptureLightingFramebuffer.setColorAttachment(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		}

		// Take the capture and apply convolution for the irradiance map (indirect diffuse liing)
		// Since we are rendering by sampling a cubemap, use a cube
		Model cubeModel = Model(Cube());
		RenderableModel renderable(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, &cubeModel, nullptr);
		m_GLCache->switchShader(m_ConvolutionShader);
		m_GLCache->setFaceCull(false);
		m_ConvolutionShader->setUniformMat4("projection", m_CubemapCamera.getProjectionMatrix());
		m_SceneCaptureCubemap.bind(0);
		m_ConvolutionShader->setUniform1i("sceneCaptureCubemap", 0);
		m_LightProbeConvolutionFramebuffer.bind();
		for (int i = 0; i < 6; i++) {
			// Setup the camera's view
			m_CubemapCamera.switchCameraToFace(i);
			m_ConvolutionShader->setUniformMat4("view", m_CubemapCamera.getViewMatrix());

			// Convolute the scene's capture and store it in the Light Probe's cubemap
			m_LightProbeConvolutionFramebuffer.setColorAttachment(lightProbe->getIrradianceMap()->getCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			renderable.draw(m_ConvolutionShader, RenderPassType::ShadowmapPassType);
			m_LightProbeConvolutionFramebuffer.setColorAttachment(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		}
		m_GLCache->setFaceCull(true);

		ProbeManager* probeManager = m_ActiveScene->getProbeManager();
		probeManager->addProbe(lightProbe);
	}

	void ProbePass::generateReflectionProbe(glm::vec3 &probePosition) {

	}

}
