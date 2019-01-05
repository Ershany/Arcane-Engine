#include "pch.h"
#include "EnvironmentProbeManager.h"

namespace arcane {

	EnvironmentProbeManager::EnvironmentProbeManager(EnvironmentProbeBlendSetting sceneProbeBlendSetting) 
		: m_ProbeBlendSetting(sceneProbeBlendSetting) 
	{}

	EnvironmentProbeManager::~EnvironmentProbeManager() {
		for (auto iter = m_Probes.begin(); iter != m_Probes.end(); ++iter) {
			delete (*iter);
		}
		m_Probes.clear();
	}

	void EnvironmentProbeManager::init(Skybox *skybox) {
		m_Skybox = skybox;
	}

	void EnvironmentProbeManager::addProbe(EnvironmentProbe *probe) {
		m_Probes.push_back(probe);
	}

	void EnvironmentProbeManager::bindProbe(glm::vec3 &renderPosition, Shader *shader) {
		// If simple blending is enabled just use the closest probe
		if (m_ProbeBlendSetting == PROBES_SIMPLE) {
			if (m_Probes.size() > 0) {
				m_Probes[0]->bind(shader);
			}
			else {
				// Fallback to skybox
				shader->setUniform1i("irradianceMap", 1);
				m_Skybox->getSkyboxCubemap()->bind(1);
			}
		}
		// If probes are disabled just use the skybox
		else if (m_ProbeBlendSetting == PROBES_DISABLED) {
			shader->setUniform1i("irradianceMap", 1);
			m_Skybox->getSkyboxCubemap()->bind(1);
		}
	}

}
