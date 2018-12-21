#include "pch.h"
#include "EnvironmentProbeManager.h"

namespace arcane {

	EnvironmentProbeManager::EnvironmentProbeManager(EnvironmentProbeBlendSetting sceneProbeBlendSetting) : m_ProbeBlendSetting(sceneProbeBlendSetting) {

	}

	EnvironmentProbeManager::~EnvironmentProbeManager() {
		for (auto iter = m_Probes.begin(); iter != m_Probes.end(); ++iter) {
			delete (*iter);
		}
		m_Probes.clear();
	}

	void EnvironmentProbeManager::addProbe(EnvironmentProbe *probe) {
		m_Probes.push_back(probe);
	}

	void EnvironmentProbeManager::bindProbe(glm::vec3 &renderPosition, Shader &shader) {
		// Testing
		if (m_Probes.size() > 0) {
			m_Probes[0]->bind(shader);
		}
	}

}
