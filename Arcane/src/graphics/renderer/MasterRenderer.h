#pragma once

#include <graphics/renderer/renderpass/deferred/DeferredGeometryPass.h>
#include <graphics/renderer/renderpass/deferred/DeferredLightingPass.h>
#include <graphics/renderer/renderpass/deferred/PostGBufferForwardPass.h>
#include <graphics/renderer/renderpass/forward/ForwardProbePass.h>
#include <graphics/renderer/renderpass/forward/ForwardLightingPass.h>
#include <graphics/renderer/renderpass/WaterPass.h>
#include <graphics/renderer/renderpass/PostProcessPass.h>
#include <graphics/renderer/renderpass/ShadowmapPass.h>
#include <scene/Scene3D.h>
#include <utils/Timer.h>

namespace Arcane
{
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
