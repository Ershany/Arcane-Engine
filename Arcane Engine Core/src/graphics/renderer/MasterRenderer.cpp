#include "pch.h"
#include "MasterRenderer.h"

#include <ui/RuntimePane.h>

namespace arcane
{

	MasterRenderer::MasterRenderer(Scene3D *scene) : m_ActiveScene(scene),
		m_ShadowmapPass(scene), m_PostProcessPass(scene), m_WaterPass(scene), m_ForwardLightingPass(scene, true), m_EnvironmentProbePass(scene),
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
#if DEBUG_PROFILING
		glFinish();
		m_ProfilingTimer.reset();
#endif // DEBUG_PROFILING
		ShadowmapPassOutput shadowmapOutput = m_ShadowmapPass.generateShadowmaps(m_ActiveScene->getCamera(), false);
#if DEBUG_PROFILING
		glFinish();
		RuntimePane::setShadowmapTimer((float)m_ProfilingTimer.elapsed());
#endif // DEBUG_PROFILING

		LightingPassOutput lightingOutput = m_ForwardLightingPass.executeLightingPass(shadowmapOutput, m_ActiveScene->getCamera(), false, true);
		WaterPassOutput waterOutput = m_WaterPass.executeWaterPass(shadowmapOutput, lightingOutput, m_ActiveScene->getCamera());
		m_PostProcessPass.executePostProcessPass(waterOutput.outputFramebuffer);


		/* Deferred Rendering */
#else
#if DEBUG_PROFILING
		glFinish();
		m_ProfilingTimer.reset();
#endif // DEBUG_PROFILING
		ShadowmapPassOutput shadowmapOutput = m_ShadowmapPass.generateShadowmaps(m_ActiveScene->getCamera(), false);
#if DEBUG_PROFILING
		glFinish();
		RuntimePane::setShadowmapTimer((float)m_ProfilingTimer.elapsed());
#endif // DEBUG_PROFILING

		GeometryPassOutput geometryOutput = m_DeferredGeometryPass.executeGeometryPass(m_ActiveScene->getCamera(), false);
		PreLightingPassOutput preLightingOutput = m_PostProcessPass.executePreLightingPass(geometryOutput, m_ActiveScene->getCamera());
		LightingPassOutput deferredLightingOutput = m_DeferredLightingPass.executeLightingPass(shadowmapOutput, geometryOutput, preLightingOutput, m_ActiveScene->getCamera(), true);
		LightingPassOutput postGBufferForward = m_PostGBufferForwardPass.executeLightingPass(shadowmapOutput, deferredLightingOutput, m_ActiveScene->getCamera(), false, true);
		WaterPassOutput waterOutput = m_WaterPass.executeWaterPass(shadowmapOutput, postGBufferForward, m_ActiveScene->getCamera());
		m_PostProcessPass.executePostProcessPass(waterOutput.outputFramebuffer);

#endif
	}

}
