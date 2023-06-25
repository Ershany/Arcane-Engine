#include "arcpch.h"
#include "MasterRenderPass.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Scene/Scene.h>
#include <Arcane/UI/RuntimePane.h>

#ifdef ARC_DEV_BUILD
#include <Arcane/Platform/OpenGL/GPUTimerManager.h>
#endif

namespace Arcane
{
	MasterRenderPass::MasterRenderPass(Scene *scene) : m_ActiveScene(scene),
		m_ShadowmapPass(scene), m_PostProcessPass(scene), m_WaterPass(scene), m_EnvironmentProbePass(scene),
		m_DeferredGeometryPass(scene), m_DeferredLightingPass(scene),
#if FORWARD_RENDER
		m_ForwardLightingPass(scene, true),
#else
		m_ForwardLightingPass(scene, false),
#endif
		m_RenderToSwapchain(true),
		m_EditorPass(scene)
	{
		m_GLCache = GLCache::GetInstance();

		m_PassthroughShader = ShaderLoader::LoadShader("post_process/Copy.glsl");
	}

	void MasterRenderPass::Init() {
		// State that should never change
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		m_EnvironmentProbePass.pregenerateIBL();
		m_EnvironmentProbePass.pregenerateProbes();

#ifdef ARC_DEV_BUILD
		m_ShadowPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Shadow Map Generation Pass (GPU)"));
		m_DeferredGeometryPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Deferred Geometry Pass (GPU)"));
		m_DeferredLightingPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Deferred Lighting Pass (GPU)"));
		m_WaterPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Water Pass (GPU)"));
		m_PostGBufferForwardPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Post GBuffer Forward Transparent Pass (GPU)"));
		m_PostProcessPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Post Process Pass (GPU)"));
		m_EditorPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Editor Pass (GPU)"));
#endif
	}

	void MasterRenderPass::Render() {
		/* Forward Rendering */
#if FORWARD_RENDER
#if DEBUG_PROFILING
		glFinish();
		m_ProfilingTimer.Reset();
#endif // DEBUG_PROFILING
		ShadowmapPassOutput shadowmapOutput = m_ShadowmapPass.GenerateShadowmaps(m_ActiveScene->GetCamera(), false);
#if DEBUG_PROFILING
		glFinish();
		RuntimePane::SetShadowmapTimer((float)m_ProfilingTimer.Elapsed());
#endif // DEBUG_PROFILING

		LightingPassOutput lightingOutput = m_ForwardLightingPass.ExecuteOpaqueLightingPass(shadowmapOutput, m_ActiveScene->GetCamera(), false, true);
		WaterPassOutput waterOutput = m_WaterPass.ExecuteWaterPass(shadowmapOutput, lightingOutput.outputFramebuffer, m_ActiveScene->GetCamera());
		LightingPassOutput postTransparencyOutput = m_ForwardLightingPass.ExecuteTransparentLightingPass(shadowmapOutput, waterOutput.outputFramebuffer, m_ActiveScene->GetCamera(), false, true);
		PostProcessPassOutput postProcessOutput = m_PostProcessPass.ExecutePostProcessPass(postTransparencyOutput.outputFramebuffer);

		Framebuffer *extraFramebuffer = postProcessOutput.outFramebuffer == m_PostProcessPass.GetFullRenderTarget() ? m_PostProcessPass.GetTonemappedNonLinearTarget() : m_PostProcessPass.GetFullRenderTarget();
		EditorPassOutput editorOutput = m_EditorPass.ExecuteEditorPass(postProcessOutput.outFramebuffer, m_PostProcessPass.GetResolveRenderTarget(), extraFramebuffer, m_ActiveScene->GetCamera());


		/* Deferred Rendering */
#else

#ifdef ARC_DEV_BUILD
		GPUTimerManager::BeginQuery(m_ShadowPassTimer);
#endif
		ShadowmapPassOutput shadowmapOutput = m_ShadowmapPass.GenerateShadowmaps(m_ActiveScene->GetCamera(), false);
#ifdef ARC_DEV_BUILD
		GPUTimerManager::EndQuery(m_ShadowPassTimer);
#endif

		GeometryPassOutput geometryOutput = m_DeferredGeometryPass.ExecuteGeometryPass(m_ActiveScene->GetCamera(), false);
		PreLightingPassOutput preLightingOutput = m_PostProcessPass.ExecutePreLightingPass(geometryOutput.outputGBuffer, m_ActiveScene->GetCamera());
		LightingPassOutput deferredLightingOutput = m_DeferredLightingPass.ExecuteLightingPass(shadowmapOutput, geometryOutput.outputGBuffer, preLightingOutput, m_ActiveScene->GetCamera(), true);
		WaterPassOutput waterOutput = m_WaterPass.ExecuteWaterPass(shadowmapOutput, deferredLightingOutput.outputFramebuffer, m_ActiveScene->GetCamera());
		LightingPassOutput postGBufferForward = m_ForwardLightingPass.ExecuteTransparentLightingPass(shadowmapOutput, waterOutput.outputFramebuffer, m_ActiveScene->GetCamera(), false, true);
		PostProcessPassOutput postProcessOutput = m_PostProcessPass.ExecutePostProcessPass(postGBufferForward.outputFramebuffer);

		Framebuffer *extraFramebuffer = postProcessOutput.outFramebuffer == m_PostProcessPass.GetFullRenderTarget() ? m_PostProcessPass.GetTonemappedNonLinearTarget() : m_PostProcessPass.GetFullRenderTarget();
		EditorPassOutput editorOutput = m_EditorPass.ExecuteEditorPass(postProcessOutput.outFramebuffer, m_PostProcessPass.GetResolveRenderTarget(), extraFramebuffer, m_ActiveScene->GetCamera());

#endif

		// Finally render the scene to the window's swapchain
		m_FinalOutputTexture = editorOutput.outFramebuffer->GetColourTexture();
		if (m_RenderToSwapchain)
		{
			Window::Bind();
			Window::ClearAll();
			m_GLCache->SetShader(m_PassthroughShader);
			m_PassthroughShader->SetUniform("input_texture", 0);
			m_FinalOutputTexture->Bind(0);
			Renderer::DrawNdcPlane();
		}
	}
}
