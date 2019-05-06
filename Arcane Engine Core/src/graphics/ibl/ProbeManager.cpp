#include "pch.h"
#include "ProbeManager.h"

namespace arcane {

	ProbeManager::ProbeManager(ProbeBlendSetting sceneProbeBlendSetting)
		: m_ProbeBlendSetting(sceneProbeBlendSetting), m_Skybox(nullptr)
	{}

	ProbeManager::~ProbeManager() {
		for (auto iter = m_LightProbes.begin(); iter != m_LightProbes.end(); ++iter) {
			delete (*iter);
		}
		for (auto iter = m_ReflectionProbes.begin(); iter != m_ReflectionProbes.end(); ++iter) {
			delete (*iter);
		}
		m_LightProbes.clear();
		m_ReflectionProbes.clear();
	}

	void ProbeManager::init(Skybox *skybox) {
		m_Skybox = skybox;
	}

	void ProbeManager::addProbe(LightProbe *probe) {
		m_LightProbes.push_back(probe);
	}

	void ProbeManager::addProbe(ReflectionProbe *probe) {
		m_ReflectionProbes.push_back(probe);
	}

	void ProbeManager::bindProbe(glm::vec3 &renderPosition, Shader *shader) {
		// If simple blending is enabled just use the closest probe
		if (m_ProbeBlendSetting == PROBES_SIMPLE) {
			// Light Probes
			if (m_LightProbes.size() > 0) {
				m_LightProbes[0]->bind(shader);
			}
			else {
				// Fallback to skybox
				shader->setUniform1i("irradianceMap", 1);
				m_Skybox->getSkyboxCubemap()->bind(1);
			}

			// Reflection Probes
			if (m_ReflectionProbes.size() > 0) {
				m_ReflectionProbes[0]->bind(shader);
			}
			else {
				// Fallback to skybox
				shader->setUniform1i("prefilterMap", 2);
				m_Skybox->getSkyboxCubemap()->bind(2);
			}
		}
		// If probes are disabled just use the skybox
		else if (m_ProbeBlendSetting == PROBES_DISABLED) {
			// Light Probes
			shader->setUniform1i("irradianceMap", 1);
			m_Skybox->getSkyboxCubemap()->bind(1);

			// Reflection Probes
			shader->setUniform1i("prefilterMap", 2);
			m_Skybox->getSkyboxCubemap()->bind(2);
		}
	}

}
