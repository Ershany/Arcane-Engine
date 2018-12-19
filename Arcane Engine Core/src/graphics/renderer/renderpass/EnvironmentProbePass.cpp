#include "pch.h"
#include "EnvironmentProbePass.h"

namespace arcane
{

	EnvironmentProbePass::EnvironmentProbePass(Scene3D *scene) : RenderPass(scene, RenderPassType::EnvironmentProbePassType),
		m_CubemapGenerationFramebuffer(DEFAULT_IBL_RESOLUTION, DEFAULT_IBL_RESOLUTION), m_ModelShader("src/shaders/pbr_model.vert", "src/shaders/pbr_model.frag"), m_ShadowmapPass(scene)
	{
		m_CubemapGenerationFramebuffer.addDepthStencilRBO(false);
	}

	EnvironmentProbePass::~EnvironmentProbePass() {}

	void EnvironmentProbePass::pregenerateProbes(ShadowmapPassOutput shadowmapData) {
		glViewport(0, 0, DEFAULT_IBL_RESOLUTION, DEFAULT_IBL_RESOLUTION);
		m_CubemapGenerationFramebuffer.bind();

		glm::vec3 probePosition = glm::vec3(120.0f, 90.0f, 140.0f);
		EnvironmentProbe *iblProbe = new EnvironmentProbe(probePosition, glm::vec2(DEFAULT_IBL_RESOLUTION, DEFAULT_IBL_RESOLUTION), true);
		iblProbe->generate();

		m_CubemapCamera.setCenterPosition(probePosition);

		for (int i = 0; i < 6; i++) {
			// Setup the view
			m_GLCache->switchShader(m_ModelShader);
			m_CubemapCamera.switchCameraToFace(i);
			m_ModelShader.setUniformMat4("projection", m_CubemapCamera.getProjectionMatrix());
			m_ModelShader.setUniformMat4("view", m_CubemapCamera.getViewMatrix());

			iblProbe->getIrradianceMap()->bind();
			m_CubemapGenerationFramebuffer.setColorAttachment(iblProbe->getIrradianceMap()->getCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		}
	}

}
