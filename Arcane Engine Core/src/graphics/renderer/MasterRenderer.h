#pragma once

#include <graphics/renderer/renderpass/deferred/DeferredGeometryPass.h>
#include <graphics/renderer/renderpass/deferred/DeferredLightingPass.h>
#include <graphics/renderer/renderpass/deferred/DeferredPostProcessPass.h>
#include <graphics/renderer/renderpass/forward/ForwardProbePass.h>
#include <graphics/renderer/renderpass/forward/ForwardLightingPass.h>
#include <graphics/renderer/renderpass/forward/ForwardPostProcessPass.h>
#include <graphics/renderer/renderpass/ShadowmapPass.h>
#include <scene/Scene3D.h>
#include <utils/Timer.h>

namespace arcane
{

	class MasterRenderer
	{
	public:
		MasterRenderer(Scene3D *scene);

		void init();
		void render();
	private:
		GLCache *m_GLCache;
		Scene3D *m_ActiveScene;

		// passes
		ShadowmapPass m_ShadowmapPass;
		ForwardProbePass m_EnvironmentProbePass;

		// Forward passes
		ForwardLightingPass m_ForwardLightingPass;
		ForwardPostProcessPass m_ForwardPostProcessPass;

		// Deferred passes
		DeferredGeometryPass m_DeferredGeometryPass;
		DeferredLightingPass m_DeferredLightingPass;
		DeferredPostProcessPass m_DeferredPostProcessPass;

		Timer m_Timer;
	};

}
