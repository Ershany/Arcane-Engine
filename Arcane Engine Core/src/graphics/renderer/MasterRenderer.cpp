#include "pch.h"
#include "MasterRenderer.h"

#include <ui/RuntimePane.h>

namespace arcane
{

	MasterRenderer::MasterRenderer(Scene3D *scene) : m_ActiveScene(scene),
		m_ShadowmapPass(scene), m_ForwardLightingPass(scene, true), m_ForwardPostProcessPass(scene), m_EnvironmentProbePass(scene), m_DeferredGeometryPass(scene), m_DeferredLightingPass(scene)
	{
		m_GLCache = GLCache::getInstance();
	}

	void MasterRenderer::init() {
		// State that should never change
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		m_EnvironmentProbePass.pregenerateIBL();
		m_EnvironmentProbePass.pregenerateProbes();
	}

	void MasterRenderer::render() {
		/*
		// Shadow map passs
#if DEBUG_ENABLED
		glFinish();
		m_Timer.reset();
#endif
		ShadowmapPassOutput shadowmapOutput = m_ShadowmapPass.generateShadowmaps(m_ActiveScene->getCamera(), false);
#if DEBUG_ENABLED
		glFinish();
		RuntimePane::setShadowmapTimer((float)m_Timer.elapsed());
#endif

		// Lighting Pass
		LightingPassOutput lightingOutput = m_ForwardLightingPass.executeRenderPass(shadowmapOutput, m_ActiveScene->getCamera(), false, true);

		// Post Process Pass
#if DEBUG_ENABLED
		glFinish();
		m_Timer.reset();
#endif
		m_ForwardPostProcessPass.executeRenderPass(lightingOutput.outputFramebuffer);
#if DEBUG_ENABLED
		glFinish();
		RuntimePane::setPostProcessTimer((float)m_Timer.elapsed());
#endif
		*/


		// Deferred Testing
		ShadowmapPassOutput shadowmapOutput = m_ShadowmapPass.generateShadowmaps(m_ActiveScene->getCamera(), false);
		GeometryPassOutput geometryOutput = m_DeferredGeometryPass.executeRenderPass(m_ActiveScene->getCamera(), false);
		LightingPassOutput deferredLightingOutput = m_DeferredLightingPass.executeRenderPass(shadowmapOutput, geometryOutput, m_ActiveScene->getCamera(), true);
		m_ForwardPostProcessPass.executeRenderPass(deferredLightingOutput.outputFramebuffer);
	}

}
