#pragma once

#include <Arcane/Graphics/Renderer/Renderpass/Deferred/DeferredGeometryPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/Deferred/DeferredLightingPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/Deferred/PostGBufferForwardPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/Forward/ForwardProbePass.h>
#include <Arcane/Graphics/Renderer/Renderpass/Forward/ForwardLightingPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/EditorPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/WaterPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/PostProcessPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/ShadowmapPass.h>
#include <Arcane/Util/Timer.h>

namespace Arcane
{
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
		PostGBufferForward m_PostGBufferForwardPass;

		// Controls
		bool m_RenderToSwapchain;

		Timer m_ProfilingTimer;
	};
}
