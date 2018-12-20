#include "pch.h"
#include "MasterRenderer.h"

#include <ui/RuntimePane.h>

namespace arcane
{

	MasterRenderer::MasterRenderer(Scene3D *scene) : m_ActiveScene(scene),
		m_ShadowmapPass(scene), m_LightingPass(scene), m_PostProcessPass(scene), m_EnvironmentProbePass(scene)
	{
		m_GLCache = GLCache::getInstance();
	}

	void MasterRenderer::init() {
		m_EnvironmentProbePass.pregenerateProbes();
	}

	void MasterRenderer::render() {
		// Shadow map passs
#if DEBUG_ENABLED
		glFinish();
		m_Timer.reset();
#endif
		ShadowmapPassOutput shadowmapOutput = m_ShadowmapPass.generateShadowmaps(m_ActiveScene->getCamera());
#if DEBUG_ENABLED
		glFinish();
		RuntimePane::setShadowmapTimer((float)m_Timer.elapsed());
#endif

		// Lighting Pass
		LightingPassOutput lightingOutput = m_LightingPass.executeRenderPass(shadowmapOutput, m_ActiveScene->getCamera());

		// Post Process Pass
#if DEBUG_ENABLED
		glFinish();
		m_Timer.reset();
#endif
		m_PostProcessPass.executeRenderPass(lightingOutput.outputFramebuffer);
#if DEBUG_ENABLED
		glFinish();
		RuntimePane::setPostProcessTimer((float)m_Timer.elapsed());
#endif
	}

}
