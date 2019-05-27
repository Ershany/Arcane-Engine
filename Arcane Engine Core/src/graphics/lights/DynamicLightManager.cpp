#include "pch.h"
#include "DynamicLightManager.h"

namespace arcane {

	// TODO: Add functionality so it can update with an entity's position and orientation
	DynamicLightManager::DynamicLightManager() 
		: m_DirectionalLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f))
	{
		init();
	}

	void DynamicLightManager::init() {
		// Setup lighting configurations
		m_DirectionalLight.m_IsActive = true;
		m_DirectionalLight.m_Direction = glm::vec3(-0.25f, -1.0f, -0.25f);
		m_DirectionalLight.m_LightColour = glm::vec3(3.25f, 3.25f, 3.25f);

		SpotLight spotlight1(glm::vec3(100.0f, 100.0f, 100.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));
		spotlight1.m_IsActive = true;
		addSpotLight(spotlight1);

		PointLight pointLight1(glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(120.0f, 95.0f, 120.0f));
		pointLight1.m_IsActive = true;
		addPointLight(pointLight1);

		PointLight pointLight2(glm::vec3(800.0f, 200.0f, 0.0f), glm::vec3(120.0f, 77.0f, 100.0f));
		pointLight2.m_IsActive = true;
		addPointLight(pointLight2);
	}

	// TODO: Dynamically change the size of the lights or use only the close lights (LIMIT OF 5 CURRENTLY FOR POINTLIGHTS)
	void DynamicLightManager::setupLightingUniforms(Shader *shader) {
		shader->setUniform1i("numSpotLights", m_SpotLights.size());
		shader->setUniform1i("numPointLights", m_PointLights.size());

		m_DirectionalLight.setupUniforms(shader, 0);
		int i = 0;
		for (auto iter = m_SpotLights.begin(); iter != m_SpotLights.end(); iter++, i++) {
			(*iter).setupUniforms(shader, i);
		}
		i = 0;
		for (auto iter = m_PointLights.begin(); iter != m_PointLights.end(); iter++, i++) {
			(*iter).setupUniforms(shader, i);
		}
	}

	void DynamicLightManager::addSpotLight(SpotLight &spotLight) {
		m_SpotLights.push_back(spotLight);
	}

	void DynamicLightManager::addPointLight(PointLight &pointLight) {
		m_PointLights.push_back(pointLight);
	}

}
