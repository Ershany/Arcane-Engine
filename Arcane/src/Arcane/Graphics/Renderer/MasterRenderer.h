#pragma once

#include <Arcane/Graphics/Renderer/Renderpass/Deferred/DeferredGeometryPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/Deferred/DeferredLightingPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/Deferred/PostGBufferForwardPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/Forward/ForwardProbePass.h>
#include <Arcane/Graphics/Renderer/Renderpass/Forward/ForwardLightingPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/WaterPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/PostProcessPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/ShadowmapPass.h>
#include <Arcane/Util/Timer.h>

namespace Arcane
{
	class Scene3D;

	class MasterRenderer
	{
	public:
		MasterRenderer(Scene3D *scene);

		void Init();
		void Render();
	private:
		GLCache *m_GLCache;
		Scene3D *m_ActiveScene;

		// Other passes
		ShadowmapPass m_ShadowmapPass;
		PostProcessPass m_PostProcessPass;
		WaterPass m_WaterPass;

		// Forward passes
		ForwardLightingPass m_ForwardLightingPass;
		ForwardProbePass m_EnvironmentProbePass;

		// Deferred passes
		DeferredGeometryPass m_DeferredGeometryPass;
		DeferredLightingPass m_DeferredLightingPass;
		PostGBufferForward m_PostGBufferForwardPass;

		Timer m_ProfilingTimer;
	};
}
