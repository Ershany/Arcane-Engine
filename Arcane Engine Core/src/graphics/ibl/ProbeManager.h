#pragma once

#include <graphics/Shader.h>
#include <graphics/Skybox.h>
#include <graphics/ibl/LightProbe.h>
#include <graphics/ibl/ReflectionProbe.h>

namespace arcane {

	enum ProbeBlendSetting
	{
		PROBES_DISABLED, // Ignores probes and uses the skybox
		PROBES_SIMPLE, // Uses the closest probe (no blending)
		PROBES_BLEND // Blends adjacent probes
	};

	class ProbeManager {
	public:
		ProbeManager(ProbeBlendSetting sceneProbeBlendSetting);
		~ProbeManager();

		void addProbe(LightProbe *probe);
		void addProbe(ReflectionProbe *probe);

		inline void setLightProbeFallback(LightProbe *probe) { m_LightProbeFallback = probe; }
		inline void setReflectionProbeFallback(ReflectionProbe *probe) { m_ReflectionProbeFallback = probe; }

		// Assumes shader is bound
		void bindProbes(glm::vec3 &renderPosition, Shader *shader);
	private:
		ProbeBlendSetting m_ProbeBlendSetting;
		
		// Scene probes
		std::vector<LightProbe*> m_LightProbes;
		std::vector<ReflectionProbe*> m_ReflectionProbes;

		// Fallback probes
		LightProbe *m_LightProbeFallback;
		ReflectionProbe *m_ReflectionProbeFallback;
	};

}
