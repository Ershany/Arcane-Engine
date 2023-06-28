#pragma once
#ifndef PROBEMANAGER_H
#define PROBEMANAGER_H

namespace Arcane
{
	class Shader;
	class LightProbe;
	class ReflectionProbe;

	enum ProbeBlendSetting
	{
		PROBES_DISABLED, // Ignores probes and uses the skybox
		PROBES_SIMPLE, // Uses the closest probe (no blending)
		PROBES_BLEND // Blends adjacent probes (TODO: Make this work)
	};

	class ProbeManager {
	public:
		ProbeManager(ProbeBlendSetting sceneProbeBlendSetting);
		~ProbeManager();

		void AddProbe(LightProbe *probe);
		void AddProbe(ReflectionProbe *probe);

		inline void SetLightProbeFallback(LightProbe *probe) { m_LightProbeFallback = probe; }
		inline void SetReflectionProbeFallback(ReflectionProbe *probe) { m_ReflectionProbeFallback = probe; }

		// Assumes shader is bound
		void BindProbes(glm::vec3 &renderPosition, Shader *shader);
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
#endif
