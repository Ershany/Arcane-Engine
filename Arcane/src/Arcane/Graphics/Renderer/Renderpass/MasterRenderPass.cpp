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
#ifdef ARC_DEV_BUILD
		GPUTimerManager::BeginQuery(m_ShadowPassTimer);
#endif
		ShadowmapPassOutput shadowmapOutput = m_ShadowmapPass.GenerateShadowmaps(m_ActiveScene->GetCamera(), false);
#ifdef ARC_DEV_BUILD
		GPUTimerManager::EndQuery(m_ShadowPassTimer);
#endif

#ifdef ARC_DEV_BUILD
		GPUTimerManager::BeginQuery(m_ForwardOpaquePassTimer);
#endif
		LightingPassOutput lightingOutput = m_ForwardLightingPass.ExecuteOpaqueLightingPass(shadowmapOutput, m_ActiveScene->GetCamera(), false, true);
#ifdef ARC_DEV_BUILD
		GPUTimerManager::EndQuery(m_ForwardOpaquePassTimer);
#endif

#ifdef ARC_DEV_BUILD
		GPUTimerManager::BeginQuery(m_WaterPassTimer);
#endif
		WaterPassOutput waterOutput = m_WaterPass.ExecuteWaterPass(shadowmapOutput, lightingOutput.outputFramebuffer, m_ActiveScene->GetCamera());
#ifdef ARC_DEV_BUILD
		GPUTimerManager::EndQuery(m_WaterPassTimer);
#endif

#ifdef ARC_DEV_BUILD
		GPUTimerManager::BeginQuery(m_ForwardTransparentPassTimer);
#endif
		LightingPassOutput postTransparencyOutput = m_ForwardLightingPass.ExecuteTransparentLightingPass(shadowmapOutput, waterOutput.outputFramebuffer, m_ActiveScene->GetCamera(), false, true);
#ifdef ARC_DEV_BUILD
		GPUTimerManager::EndQuery(m_ForwardTransparentPassTimer);
#endif

#ifdef ARC_DEV_BUILD
		GPUTimerManager::BeginQuery(m_PostProcessPassTimer);
#endif
		PostProcessPassOutput postProcessOutput = m_PostProcessPass.ExecutePostProcessPass(postTransparencyOutput.outputFramebuffer);
#ifdef ARC_DEV_BUILD
		GPUTimerManager::EndQuery(m_PostProcessPassTimer);
#endif

#ifdef ARC_DEV_BUILD
		GPUTimerManager::BeginQuery(m_EditorPassTimer);
#endif
		Framebuffer *extraFramebuffer = postProcessOutput.outFramebuffer == m_PostProcessPass.GetFullRenderTarget() ? m_PostProcessPass.GetTonemappedNonLinearTarget() : m_PostProcessPass.GetFullRenderTarget();
		EditorPassOutput editorOutput = m_EditorPass.ExecuteEditorPass(postProcessOutput.outFramebuffer, m_PostProcessPass.GetResolveRenderTarget(), extraFramebuffer, m_ActiveScene->GetCamera());
#ifdef ARC_DEV_BUILD
		GPUTimerManager::EndQuery(m_EditorPassTimer);
#endif


#else
		/* Deferred Rendering */
#ifdef ARC_DEV_BUILD
		GPUTimerManager::BeginQuery(m_ShadowPassTimer);
#endif
		ShadowmapPassOutput shadowmapOutput = m_ShadowmapPass.GenerateShadowmaps(m_ActiveScene->GetCamera(), false);
#ifdef ARC_DEV_BUILD
		GPUTimerManager::EndQuery(m_ShadowPassTimer);
#endif

#ifdef ARC_DEV_BUILD
		GPUTimerManager::BeginQuery(m_DeferredGeometryPassTimer);
#endif
		GeometryPassOutput geometryOutput = m_DeferredGeometryPass.ExecuteGeometryPass(m_ActiveScene->GetCamera(), false);
#ifdef ARC_DEV_BUILD
		GPUTimerManager::EndQuery(m_DeferredGeometryPassTimer);
#endif

#ifdef ARC_DEV_BUILD
		GPUTimerManager::BeginQuery(m_SSAOPassTimer);
#endif
		PreLightingPassOutput preLightingOutput = m_PostProcessPass.ExecutePreLightingPass(geometryOutput.outputGBuffer, m_ActiveScene->GetCamera());
#ifdef ARC_DEV_BUILD
		GPUTimerManager::EndQuery(m_SSAOPassTimer);
#endif

#ifdef ARC_DEV_BUILD
		GPUTimerManager::BeginQuery(m_DeferredLightingPassTimer);
#endif
		LightingPassOutput deferredLightingOutput = m_DeferredLightingPass.ExecuteLightingPass(shadowmapOutput, geometryOutput.outputGBuffer, preLightingOutput, m_ActiveScene->GetCamera(), true);
#ifdef ARC_DEV_BUILD
		GPUTimerManager::EndQuery(m_DeferredLightingPassTimer);
#endif

#ifdef ARC_DEV_BUILD
		GPUTimerManager::BeginQuery(m_WaterPassTimer);
#endif
		WaterPassOutput waterOutput = m_WaterPass.ExecuteWaterPass(shadowmapOutput, deferredLightingOutput.outputFramebuffer, m_ActiveScene->GetCamera());
#ifdef ARC_DEV_BUILD
		GPUTimerManager::EndQuery(m_WaterPassTimer);
#endif

#ifdef ARC_DEV_BUILD
		GPUTimerManager::BeginQuery(m_PostGBufferForwardPassTimer);
#endif
		LightingPassOutput postGBufferForward = m_ForwardLightingPass.ExecuteTransparentLightingPass(shadowmapOutput, waterOutput.outputFramebuffer, m_ActiveScene->GetCamera(), false, true);
#ifdef ARC_DEV_BUILD
		GPUTimerManager::EndQuery(m_PostGBufferForwardPassTimer);
#endif

#ifdef ARC_DEV_BUILD
		GPUTimerManager::BeginQuery(m_PostProcessPassTimer);
#endif
		PostProcessPassOutput postProcessOutput = m_PostProcessPass.ExecutePostProcessPass(postGBufferForward.outputFramebuffer);
#ifdef ARC_DEV_BUILD
		GPUTimerManager::EndQuery(m_PostProcessPassTimer);
#endif

#ifdef ARC_DEV_BUILD
		GPUTimerManager::BeginQuery(m_EditorPassTimer);
#endif
		Framebuffer *extraFramebuffer = postProcessOutput.outFramebuffer == m_PostProcessPass.GetFullRenderTarget() ? m_PostProcessPass.GetTonemappedNonLinearTarget() : m_PostProcessPass.GetFullRenderTarget();
		EditorPassOutput editorOutput = m_EditorPass.ExecuteEditorPass(postProcessOutput.outFramebuffer, m_PostProcessPass.GetResolveRenderTarget(), extraFramebuffer, m_ActiveScene->GetCamera());
#ifdef ARC_DEV_BUILD
		GPUTimerManager::EndQuery(m_EditorPassTimer);
#endif

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
