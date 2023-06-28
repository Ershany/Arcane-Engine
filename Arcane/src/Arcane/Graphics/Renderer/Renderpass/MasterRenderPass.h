#pragma once
#ifndef MASTERRENDERPASS_H
#define MASTERRENDERPASS_H

#ifndef DEFERREDGEOMETRYPASS_H
#include <Arcane/Graphics/Renderer/Renderpass/Deferred/DeferredGeometryPass.h>
#endif

#ifndef DEFERREDLIGHTINGPASS_H
#include <Arcane/Graphics/Renderer/Renderpass/Deferred/DeferredLightingPass.h>
#endif

#ifndef FORWARDPROBEPASS_H
#include <Arcane/Graphics/Renderer/Renderpass/Forward/ForwardProbePass.h>
#endif

#ifndef FORWARDLIGHTINGPASS_H
#include <Arcane/Graphics/Renderer/Renderpass/Forward/ForwardLightingPass.h>
#endif

#ifndef EDITORPASS_H
#include <Arcane/Graphics/Renderer/Renderpass/EditorPass.h>
#endif

#ifndef WATERPASS_H
#include <Arcane/Graphics/Renderer/Renderpass/WaterPass.h>
#endif

#ifndef POSTPROCESSPASS_H
#include <Arcane/Graphics/Renderer/Renderpass/PostProcessPass.h>
#endif

#ifndef SHADOWMAPPASS_H
#include <Arcane/Graphics/Renderer/Renderpass/ShadowmapPass.h>
#endif

namespace Arcane
{
	class GPUTimer;
	class Scene;
	class Shader;

	class MasterRenderPass
	{
	public:
		MasterRenderPass(Scene *scene);

		void Init();
		void Render();

		inline void SetRenderToSwapchain(bool choice) { m_RenderToSwapchain = choice; }

		inline Texture* GetFinalOutputTexture() { return m_FinalOutputTexture; }
		inline PostProcessPass* GetPostProcessPass() { return &m_PostProcessPass; }
		inline EditorPass* GetEditorPass() { return &m_EditorPass; }
	private:
		GLCache *m_GLCache;
		Scene *m_ActiveScene;

		Texture *m_FinalOutputTexture;
		Shader *m_PassthroughShader;

		// Other passes
		ShadowmapPass m_ShadowmapPass;
		PostProcessPass m_PostProcessPass;
		WaterPass m_WaterPass;
		EditorPass m_EditorPass;

		// Forward passes
		ForwardLightingPass m_ForwardLightingPass;
		ForwardProbePass m_EnvironmentProbePass;

		// Deferred passes
		DeferredGeometryPass m_DeferredGeometryPass;
		DeferredLightingPass m_DeferredLightingPass;

		// Controls
		bool m_RenderToSwapchain;

#ifdef ARC_DEV_BUILD
	#if FORWARD_RENDER
		GPUTimer *m_ShadowPassTimer, *m_ForwardOpaquePassTimer, *m_WaterPassTimer, *m_ForwardTransparentPassTimer, *m_PostProcessPassTimer, *m_EditorPassTimer;
	#else
		GPUTimer *m_ShadowPassTimer, *m_DeferredGeometryPassTimer, *m_SSAOPassTimer, *m_DeferredLightingPassTimer, *m_WaterPassTimer, *m_PostGBufferForwardPassTimer, *m_PostProcessPassTimer, *m_EditorPassTimer;
	#endif
#endif
	};
}
#endif
