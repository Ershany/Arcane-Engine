#pragma once

#include <graphics/renderer/renderpass/EnvironmentProbePass.h>
#include <graphics/renderer/renderpass/LightingPass.h>
#include <graphics/renderer/renderpass/PostProcessPass.h>
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

		// Render passes
		ShadowmapPass m_ShadowmapPass;
		LightingPass m_LightingPass;
		PostProcessPass m_PostProcessPass;
		EnvironmentProbePass m_EnvironmentProbePass;

		Timer m_Timer;
	};

}
