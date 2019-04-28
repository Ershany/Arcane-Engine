#include "pch.h"
#include "EnvironmentProbePass.h"

#include <graphics/renderer/renderpass/LightingPass.h>
#include <graphics/renderer/renderpass/ShadowmapPass.h>

namespace arcane
{

	EnvironmentProbePass::EnvironmentProbePass(Scene3D *scene) : RenderPass(scene, RenderPassType::EnvironmentProbePassType),
		m_CubemapGenerationFramebuffer(DEFAULT_IBL_RESOLUTION, DEFAULT_IBL_RESOLUTION)
	{
		m_CubemapGenerationFramebuffer.addDepthStencilRBO(false);
	}

	EnvironmentProbePass::~EnvironmentProbePass() {}

	void EnvironmentProbePass::pregenerateProbes() {
		// Create framebuffers for generating the probes
		Framebuffer shadowmapFramebuffer(DEFAULT_IBL_RESOLUTION, DEFAULT_IBL_RESOLUTION);
		shadowmapFramebuffer.addDepthAttachment(false).createFramebuffer();
		Framebuffer lightingFramebuffer(DEFAULT_IBL_RESOLUTION, DEFAULT_IBL_RESOLUTION);
		lightingFramebuffer.addTexture2DColorAttachment(false).addDepthStencilRBO(false).createFramebuffer();
		
		// Generate the cubemap for the probe
		glm::vec3 probePosition = glm::vec3(67.0f, 92.0f, 133.0f);
		EnvironmentProbe *iblProbe = new EnvironmentProbe(probePosition, glm::vec2(DEFAULT_IBL_RESOLUTION, DEFAULT_IBL_RESOLUTION), true);
		iblProbe->generate();

		// Initialize step before rendering to the probe's cubemap
		m_CubemapCamera.setCenterPosition(probePosition);
		ShadowmapPass shadowPass(m_ActiveScene, &shadowmapFramebuffer);
		LightingPass lightingPass(m_ActiveScene, &lightingFramebuffer, false);

		// Render to the probe's cubemap
		for (int i = 0; i < 6; i++) {
			// Setup the camera's view
			m_CubemapCamera.switchCameraToFace(i);

			// Shadow pass
			ShadowmapPassOutput shadowpassOutput = shadowPass.generateShadowmaps(&m_CubemapCamera);

			// Light pass
			lightingFramebuffer.bind();
			lightingFramebuffer.setColorAttachment(iblProbe->getIrradianceMap()->getCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			lightingPass.executeRenderPass(shadowpassOutput, &m_CubemapCamera);
		}

		EnvironmentProbeManager *probeManager = m_ActiveScene->getProbeManager();
		probeManager->addProbe(iblProbe);
	}

}
