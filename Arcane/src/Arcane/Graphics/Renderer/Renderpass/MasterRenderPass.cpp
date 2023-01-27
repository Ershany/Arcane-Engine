#include "arcpch.h"
#include "MasterRenderPass.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Scene/Scene.h>
#include <Arcane/UI/RuntimePane.h>

namespace Arcane
{
	MasterRenderPass::MasterRenderPass(Scene *scene) : m_ActiveScene(scene),
		m_ShadowmapPass(scene), m_PostProcessPass(scene), m_WaterPass(scene), m_ForwardLightingPass(scene, true), m_EnvironmentProbePass(scene),
		m_DeferredGeometryPass(scene), m_DeferredLightingPass(scene), m_PostGBufferForwardPass(scene),
		m_RenderToSwapchain(true)
	{
		m_GLCache = GLCache::GetInstance();

		m_PassthroughShader = ShaderLoader::LoadShader("post_process/Copy.glsl");
	}

	void MasterRenderPass::Init() {
		// State that should never change
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		m_EnvironmentProbePass.pregenerateIBL();
		m_EnvironmentProbePass.pregenerateProbes();
	}

	void MasterRenderPass::Render() {
		/* Forward Rendering */
#if FORWARD_RENDER
#if DEBUG_PROFILING
		glFinish();
		m_ProfilingTimer.Reset();
#endif // DEBUG_PROFILING
		ShadowmapPassOutput shadowmapOutput = m_ShadowmapPass.generateShadowmaps(m_ActiveScene->GetCamera(), false);
#if DEBUG_PROFILING
		glFinish();
		RuntimePane::SetShadowmapTimer((float)m_ProfilingTimer.Elapsed());
#endif // DEBUG_PROFILING

		LightingPassOutput lightingOutput = m_ForwardLightingPass.executeLightingPass(shadowmapOutput, m_ActiveScene->GetCamera(), false, true);
		WaterPassOutput waterOutput = m_WaterPass.executeWaterPass(shadowmapOutput, lightingOutput.outputFramebuffer, m_ActiveScene->GetCamera());
		PostProcessPassOutput postProcessOutput = m_PostProcessPass.executePostProcessPass(waterOutput.outputFramebuffer);


		/* Deferred Rendering */
#else
#if DEBUG_PROFILING
		glFinish();
		m_ProfilingTimer.Reset();
#endif // DEBUG_PROFILING
		ShadowmapPassOutput shadowmapOutput = m_ShadowmapPass.generateShadowmaps(m_ActiveScene->GetCamera(), false);
#if DEBUG_PROFILING
		glFinish();
		RuntimePane::SetShadowmapTimer((float)m_ProfilingTimer.Elapsed());
#endif // DEBUG_PROFILING

		GeometryPassOutput geometryOutput = m_DeferredGeometryPass.executeGeometryPass(m_ActiveScene->GetCamera(), false);
		PreLightingPassOutput preLightingOutput = m_PostProcessPass.executePreLightingPass(geometryOutput.outputGBuffer, m_ActiveScene->GetCamera());
		LightingPassOutput deferredLightingOutput = m_DeferredLightingPass.executeLightingPass(shadowmapOutput, geometryOutput.outputGBuffer, preLightingOutput, m_ActiveScene->GetCamera(), true);
		WaterPassOutput waterOutput = m_WaterPass.executeWaterPass(shadowmapOutput, deferredLightingOutput.outputFramebuffer, m_ActiveScene->GetCamera());
		LightingPassOutput postGBufferForward = m_PostGBufferForwardPass.executeLightingPass(shadowmapOutput, waterOutput.outputFramebuffer, m_ActiveScene->GetCamera(), false, true);
		PostProcessPassOutput postProcessOutput = m_PostProcessPass.executePostProcessPass(postGBufferForward.outputFramebuffer);

#endif

		// Finally render the scene to the window's swapchain
		m_FinalOutputTexture = postProcessOutput.outFramebuffer->GetColourTexture();
		if (m_RenderToSwapchain)
		{
			Window::Bind();
			Window::Clear();
			m_GLCache->SetShader(m_PassthroughShader);
			m_PassthroughShader->SetUniform("input_texture", 0);
			m_FinalOutputTexture->Bind(0);
			Renderer::DrawNdcPlane();
		}
	}
}
