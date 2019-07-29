#include "pch.h"
#include "MasterRenderer.h"

#include <ui/RuntimePane.h>

namespace arcane
{

	MasterRenderer::MasterRenderer(Scene3D *scene) : m_ActiveScene(scene),
		m_ShadowmapPass(scene), m_PostProcessPass(scene), m_ForwardLightingPass(scene, true), m_EnvironmentProbePass(scene),
		m_DeferredGeometryPass(scene), m_DeferredLightingPass(scene), m_PostGBufferForwardPass(scene)
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
		/* Forward Rendering */
#if FORWARD_RENDER
#if DEBUG_ENABLED
		glFinish();
		m_Timer.reset();
#endif
		ShadowmapPassOutput shadowmapOutput = m_ShadowmapPass.generateShadowmaps(m_ActiveScene->getCamera(), false);
#if DEBUG_ENABLED
		glFinish();
		RuntimePane::setShadowmapTimer((float)m_Timer.elapsed());
#endif

		LightingPassOutput lightingOutput = m_ForwardLightingPass.executeLightingPass(shadowmapOutput, m_ActiveScene->getCamera(), false, true);
		m_PostProcessPass.executePostProcessPass(lightingOutput.outputFramebuffer);


		/* Deferred Rendering */
#else
#if DEBUG_ENABLED
		glFinish();
		m_Timer.reset();
#endif
		ShadowmapPassOutput shadowmapOutput = m_ShadowmapPass.generateShadowmaps(m_ActiveScene->getCamera(), false);
#if DEBUG_ENABLED
		glFinish();
		RuntimePane::setShadowmapTimer((float)m_Timer.elapsed());
#endif

		GeometryPassOutput geometryOutput = m_DeferredGeometryPass.executeGeometryPass(m_ActiveScene->getCamera(), false);
		PreLightingPassOutput preLightingOutput = m_PostProcessPass.executePreLightingPass(geometryOutput, m_ActiveScene->getCamera());
		LightingPassOutput deferredLightingOutput = m_DeferredLightingPass.executeLightingPass(shadowmapOutput, geometryOutput, preLightingOutput, m_ActiveScene->getCamera(), true);
		LightingPassOutput postGBufferForward = m_PostGBufferForwardPass.executeLightingPass(shadowmapOutput, deferredLightingOutput, m_ActiveScene->getCamera(), false, true);
		m_PostProcessPass.executePostProcessPass(postGBufferForward.outputFramebuffer);

#endif
	}

}
