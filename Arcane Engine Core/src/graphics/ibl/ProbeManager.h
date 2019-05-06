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

		void init(Skybox *skybox);

		void addProbe(LightProbe *probe);
		void addProbe(ReflectionProbe *probe);

		// Assumes shader is bound
		void bindProbe(glm::vec3 &renderPosition, Shader *shader);
	private:
		ProbeBlendSetting m_ProbeBlendSetting;
		std::vector<LightProbe*> m_LightProbes;
		std::vector<ReflectionProbe*> m_ReflectionProbes;

		Skybox *m_Skybox;
	};

}
