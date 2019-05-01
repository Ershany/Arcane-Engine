#include "pch.h"
#include "EnvironmentProbePass.h"

#include <graphics/mesh/common/Cube.h>
#include <graphics/renderer/renderpass/LightingPass.h>
#include <graphics/renderer/renderpass/ShadowmapPass.h>
#include <utils/loaders/ShaderLoader.h>

namespace arcane
{

	EnvironmentProbePass::EnvironmentProbePass(Scene3D *scene) : RenderPass(scene, RenderPassType::EnvironmentProbePassType),
		m_CaptureShadowFramebuffer(DEFAULT_IBL_RESOLUTION, DEFAULT_IBL_RESOLUTION), m_CaptureLightingFramebuffer(DEFAULT_IBL_RESOLUTION, DEFAULT_IBL_RESOLUTION),
		m_ConvolutionFramebuffer(LIGHT_PROBE_RESOLUTION, LIGHT_PROBE_RESOLUTION)
	{
		m_CaptureShadowFramebuffer.addDepthAttachment(false).createFramebuffer();
		m_CaptureLightingFramebuffer.addTexture2DColorAttachment(false).addDepthStencilRBO(false).createFramebuffer();
		m_ConvolutionFramebuffer.addTexture2DColorAttachment(false).addDepthStencilRBO(false).createFramebuffer();

		m_ConvolutionShader = ShaderLoader::loadShader("src/shaders/lightprobe_convolution.vert", "src/shaders/lightprobe_convolution.frag");
	}

	EnvironmentProbePass::~EnvironmentProbePass() {}

	void EnvironmentProbePass::pregenerateProbes() {
		// Generate the cubemap for the probe
		glm::vec3 probePosition = glm::vec3(67.0f, 92.0f, 133.0f);
		EnvironmentProbe *iblProbe = new EnvironmentProbe(probePosition, glm::vec2(DEFAULT_IBL_RESOLUTION, DEFAULT_IBL_RESOLUTION), true);
		iblProbe->generate();

		// Initialize step before rendering to the probe's cubemap
		m_CubemapCamera.setCenterPosition(probePosition);
		ShadowmapPass shadowPass(m_ActiveScene, &m_CaptureShadowFramebuffer);
		LightingPass lightingPass(m_ActiveScene, &m_CaptureLightingFramebuffer, false);

		// Render the environment to the probe's cubemap
		for (int i = 0; i < 6; i++) {
			// Setup the camera's view
			m_CubemapCamera.switchCameraToFace(i);

			// Shadow pass
			ShadowmapPassOutput shadowpassOutput = shadowPass.generateShadowmaps(&m_CubemapCamera);

			// Light pass
			m_CaptureLightingFramebuffer.bind();
			m_CaptureLightingFramebuffer.setColorAttachment(iblProbe->getIrradianceMap()->getCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			lightingPass.executeRenderPass(shadowpassOutput, &m_CubemapCamera);
			m_CaptureLightingFramebuffer.setColorAttachment(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		}

		// Take the capture and apply convolution for the irradiance map (indirect diffuse lighting)
		// Since we are rendering to and from a cubemap, use a cube for this process
		Model cubeModel = Model(Cube());
		RenderableModel renderable(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, &cubeModel, nullptr);
		m_GLCache->switchShader(m_ConvolutionShader);
		m_ConvolutionShader->setUniformMat4("projection", m_CubemapCamera.getProjectionMatrix());
		m_ConvolutionFramebuffer.bind();
		for (int i = 0; i < 6; i++) {
			// Setup the camera's view
			m_CubemapCamera.switchCameraToFace(i);
			m_ConvolutionShader->setUniformMat4("view", m_CubemapCamera.getViewMatrix());

			// Set the probe's irradiance cubemap face as the render target on the framebuffer
			//m_ConvolutionFramebuffer.setColorAttachment(iblProbe->getIrradianceMap()->getCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			//todo: send the samplerCube to the shader

			// Finally convolute the captured scene
			//renderable.draw(m_ConvolutionShader, RenderPassType::ShadowmapPassType);
		}

		EnvironmentProbeManager *probeManager = m_ActiveScene->getProbeManager();
		probeManager->addProbe(iblProbe);
	}

}
