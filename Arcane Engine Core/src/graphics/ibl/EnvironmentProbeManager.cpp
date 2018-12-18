#include "pch.h"
#include "EnvironmentProbeManager.h"

namespace arcane
{

	EnvironmentProbeManager::EnvironmentProbeManager() : m_CubemapGenerationFramebuffer(DEFAULT_IBL_RESOLUTION, DEFAULT_IBL_RESOLUTION) {

	}

	void EnvironmentProbeManager::GenerateProbe(glm::vec3 &worldPosition, glm::vec2 &probeResolution, bool isStaticProbe) {
		EnvironmentProbe *iblProbe = new EnvironmentProbe(worldPosition, probeResolution, isStaticProbe);

		m_CubemapGenerationFramebuffer = Framebuffer((unsigned int)probeResolution.x, (unsigned int)probeResolution.y);
		m_CubemapGenerationFramebuffer.addDepthStencilRBO(false);
	}

}
