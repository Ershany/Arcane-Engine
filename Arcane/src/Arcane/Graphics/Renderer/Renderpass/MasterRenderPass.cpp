#include "arcpch.h"
#include "MasterRenderPass.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>
#include <Arcane/Scene/Scene.h>

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
	#if FORWARD_RENDER
		m_ShadowPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Shadow Map Generation Pass (GPU)"));
		m_ForwardOpaquePassTimer = GPUTimerManager::CreateGPUTimer(std::string("Forward Opaque Pass (GPU)"));
		m_WaterPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Water Pass (GPU)"));
		m_ForwardTransparentPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Forward Transparent Pass (GPU)"));
		m_PostProcessPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Post Process Pass (GPU)"));
		m_EditorPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Editor Pass (GPU)"));
	#else
		m_ShadowPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Shadow Map Generation Pass (GPU)"));
		m_DeferredGeometryPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Deferred Geometry Pass (GPU)"));
		m_SSAOPassTimer = GPUTimerManager::CreateGPUTimer(std::string("SSAO Pass (GPU)"));
		m_DeferredLightingPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Deferred Lighting Pass (GPU)"));
		m_WaterPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Water Pass (GPU)"));
		m_PostGBufferForwardPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Post GBuffer Forward Transparent Pass (GPU)"));
		m_PostProcessPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Post Process Pass (GPU)"));
		m_EditorPassTimer = GPUTimerManager::CreateGPUTimer(std::string("Editor Pass (GPU)"));
	#endif
#endif
	}

	void MasterRenderPass::Render() {
#if FORWARD_RENDER
		/* Forward Rendering */
		ARC_PUSH_RENDER_TAG("Shadow Pass");
		ARC_GPU_TIMER_BEGIN(m_ShadowPassTimer);
		ShadowmapPassOutput shadowmapOutput = m_ShadowmapPass.GenerateShadowmaps(m_ActiveScene->GetCamera(), false);
		ARC_GPU_TIMER_END(m_ShadowPassTimer);
		ARC_POP_RENDER_TAG();

		ARC_PUSH_RENDER_TAG("Forward Opaque Pass");
		ARC_GPU_TIMER_BEGIN(m_ForwardOpaquePassTimer);
		LightingPassOutput lightingOutput = m_ForwardLightingPass.ExecuteOpaqueLightingPass(shadowmapOutput, m_ActiveScene->GetCamera(), false, true);
		ARC_GPU_TIMER_END(m_ForwardOpaquePassTimer);
		ARC_POP_RENDER_TAG();

		ARC_PUSH_RENDER_TAG("Water Pass");
		ARC_GPU_TIMER_BEGIN(m_WaterPassTimer);
		WaterPassOutput waterOutput = m_WaterPass.ExecuteWaterPass(shadowmapOutput, lightingOutput.outputFramebuffer, m_ActiveScene->GetCamera());
		ARC_GPU_TIMER_END(m_WaterPassTimer);
		ARC_POP_RENDER_TAG();

		ARC_PUSH_RENDER_TAG("Forward Transparent Pass");
		ARC_GPU_TIMER_BEGIN(m_ForwardTransparentPassTimer);
		LightingPassOutput postTransparencyOutput = m_ForwardLightingPass.ExecuteTransparentLightingPass(shadowmapOutput, waterOutput.outputFramebuffer, m_ActiveScene->GetCamera(), false, true);
		ARC_GPU_TIMER_END(m_ForwardTransparentPassTimer);
		ARC_POP_RENDER_TAG();

		ARC_PUSH_RENDER_TAG("Post Process Pass");
		ARC_GPU_TIMER_BEGIN(m_PostProcessPassTimer);
		PostProcessPassOutput postProcessOutput = m_PostProcessPass.ExecutePostProcessPass(postTransparencyOutput.outputFramebuffer);
		ARC_GPU_TIMER_END(m_PostProcessPassTimer);
		ARC_POP_RENDER_TAG();

		ARC_PUSH_RENDER_TAG("Editor Pass");
		ARC_GPU_TIMER_BEGIN(m_EditorPassTimer);
		Framebuffer *extraFramebuffer = postProcessOutput.outFramebuffer == m_PostProcessPass.GetFullRenderTarget() ? m_PostProcessPass.GetTonemappedNonLinearTarget() : m_PostProcessPass.GetFullRenderTarget();
		EditorPassOutput editorOutput = m_EditorPass.ExecuteEditorPass(postProcessOutput.outFramebuffer, m_PostProcessPass.GetResolveRenderTarget(), extraFramebuffer, m_ActiveScene->GetCamera());
		ARC_GPU_TIMER_END(m_EditorPassTimer);
		ARC_POP_RENDER_TAG();
#else
		/* Deferred Rendering */
		ARC_PUSH_RENDER_TAG("Shadow Pass");
		ARC_GPU_TIMER_BEGIN(m_ShadowPassTimer);
		ShadowmapPassOutput shadowmapOutput = m_ShadowmapPass.GenerateShadowmaps(m_ActiveScene->GetCamera(), false);
		ARC_GPU_TIMER_END(m_ShadowPassTimer);
		ARC_POP_RENDER_TAG();

		ARC_PUSH_RENDER_TAG("Deferred Geometry Pass (Opaque)");
		ARC_GPU_TIMER_BEGIN(m_DeferredGeometryPassTimer);
		GeometryPassOutput geometryOutput = m_DeferredGeometryPass.ExecuteGeometryPass(m_ActiveScene->GetCamera(), false);
		ARC_GPU_TIMER_END(m_DeferredGeometryPassTimer);
		ARC_POP_RENDER_TAG();

		ARC_PUSH_RENDER_TAG("Pre-Lighting Pass");
		ARC_GPU_TIMER_BEGIN(m_SSAOPassTimer);
		PreLightingPassOutput preLightingOutput = m_PostProcessPass.ExecutePreLightingPass(geometryOutput.outputGBuffer, m_ActiveScene->GetCamera());
		ARC_GPU_TIMER_END(m_SSAOPassTimer);
		ARC_POP_RENDER_TAG();

		ARC_PUSH_RENDER_TAG("Deferred Lighting Pass");
		ARC_GPU_TIMER_BEGIN(m_DeferredLightingPassTimer);
		LightingPassOutput deferredLightingOutput = m_DeferredLightingPass.ExecuteLightingPass(shadowmapOutput, geometryOutput.outputGBuffer, preLightingOutput, m_ActiveScene->GetCamera(), true);
		ARC_GPU_TIMER_END(m_DeferredLightingPassTimer);
		ARC_POP_RENDER_TAG();

		ARC_PUSH_RENDER_TAG("Water Pass");
		ARC_GPU_TIMER_BEGIN(m_WaterPassTimer);
		WaterPassOutput waterOutput = m_WaterPass.ExecuteWaterPass(shadowmapOutput, deferredLightingOutput.outputFramebuffer, m_ActiveScene->GetCamera());
		ARC_GPU_TIMER_END(m_WaterPassTimer);
		ARC_POP_RENDER_TAG();

		ARC_PUSH_RENDER_TAG("Post GBuffer Forward Pass (Transparent)");
		ARC_GPU_TIMER_BEGIN(m_PostGBufferForwardPassTimer);
		LightingPassOutput postGBufferForward = m_ForwardLightingPass.ExecuteTransparentLightingPass(shadowmapOutput, waterOutput.outputFramebuffer, m_ActiveScene->GetCamera(), false, true);
		ARC_GPU_TIMER_END(m_PostGBufferForwardPassTimer);
		ARC_POP_RENDER_TAG();

		ARC_PUSH_RENDER_TAG("Post Process Pass");
		ARC_GPU_TIMER_BEGIN(m_PostProcessPassTimer);
		PostProcessPassOutput postProcessOutput = m_PostProcessPass.ExecutePostProcessPass(postGBufferForward.outputFramebuffer);
		ARC_GPU_TIMER_END(m_PostProcessPassTimer);
		ARC_POP_RENDER_TAG();

		ARC_PUSH_RENDER_TAG("Editor Pass");
		ARC_GPU_TIMER_BEGIN(m_EditorPassTimer);
		Framebuffer *extraFramebuffer = postProcessOutput.outFramebuffer == m_PostProcessPass.GetFullRenderTarget() ? m_PostProcessPass.GetTonemappedNonLinearTarget() : m_PostProcessPass.GetFullRenderTarget();
		EditorPassOutput editorOutput = m_EditorPass.ExecuteEditorPass(postProcessOutput.outFramebuffer, m_PostProcessPass.GetResolveRenderTarget(), extraFramebuffer, m_ActiveScene->GetCamera());
		ARC_GPU_TIMER_END(m_EditorPassTimer);
		ARC_POP_RENDER_TAG();
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
