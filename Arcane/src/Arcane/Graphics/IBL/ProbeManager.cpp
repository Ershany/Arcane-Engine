#include "arcpch.h"
#include "ProbeManager.h"

#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Skybox.h>
#include <Arcane/Graphics/IBL/LightProbe.h>
#include <Arcane/Graphics/IBL/ReflectionProbe.h>

namespace Arcane
{
	ProbeManager::ProbeManager(ProbeBlendSetting sceneProbeBlendSetting)
		: m_ProbeBlendSetting(sceneProbeBlendSetting), m_LightProbeFallback(nullptr), m_ReflectionProbeFallback(nullptr)
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

		delete m_LightProbeFallback;
		delete m_ReflectionProbeFallback;
	}

	void ProbeManager::AddProbe(LightProbe *probe) {
		m_LightProbes.push_back(probe);
	}

	void ProbeManager::AddProbe(ReflectionProbe *probe) {
		m_ReflectionProbes.push_back(probe);
	}

	void ProbeManager::BindProbes(glm::vec3 &renderPosition, Shader *shader) {
		// If simple blending is enabled just use the closest probe
		if (m_ProbeBlendSetting == PROBES_SIMPLE) {
			// Light Probes
			if (m_LightProbes.size() > 0) {
				unsigned int closestIndex = 0;
				for (unsigned int i = 1; i < m_LightProbes.size(); i++) {
					if (glm::length2(m_LightProbes[i]->GetPosition() - renderPosition) < glm::length2(m_LightProbes[closestIndex]->GetPosition() - renderPosition))
						closestIndex = i;
				}
				m_LightProbes[closestIndex]->Bind(shader);
			}
			// Light probe fallback
			else {
				m_LightProbeFallback->Bind(shader);
			}

			// Reflection Probes
			if (m_ReflectionProbes.size() > 0) {
				unsigned int closestIndex = 0;
				for (unsigned int i = 1; i < m_ReflectionProbes.size(); i++) {
					if (glm::length2(m_ReflectionProbes[i]->GetPosition() - renderPosition) < glm::length2(m_ReflectionProbes[closestIndex]->GetPosition() - renderPosition))
						closestIndex = i;
				}
				m_ReflectionProbes[closestIndex]->Bind(shader);
			}
			// Reflection probe fallback
			else {
				m_ReflectionProbeFallback->Bind(shader);
			}
		}
		// If probes are disabled just use the skybox
		else if (m_ProbeBlendSetting == PROBES_DISABLED) {
			// Light probe fallback
			m_LightProbeFallback->Bind(shader);

			// Reflection probe fallback
			m_ReflectionProbeFallback->Bind(shader);
		}
	}
}
