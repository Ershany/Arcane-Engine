#include "pch.h"
#include "DynamicLightManager.h"

namespace arcane {

	DynamicLightManager::DynamicLightManager() {
		init();
	}

	void DynamicLightManager::init() {
		// Setup some lights for the scene
		DirectionalLight directionalLight1(glm::vec3(3.25f, 3.25f, 3.25f), glm::vec3(-0.25f, -1.0f, -0.25f));
		directionalLight1.m_IsStatic = true;
		addDirectionalLight(directionalLight1);

		SpotLight spotlight1(glm::vec3(100.0f, 100.0f, 100.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));
		spotlight1.m_IsStatic = false;
		addSpotLight(spotlight1);

		PointLight pointLight1(glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(120.0f, 95.0f, 120.0f));
		pointLight1.m_IsStatic = false;
		addPointLight(pointLight1);

		PointLight pointLight2(glm::vec3(800.0f, 200.0f, 0.0f), glm::vec3(120.0f, 77.0f, 100.0f));
		pointLight2.m_IsStatic = true;
		addPointLight(pointLight2);
	}

	void DynamicLightManager::bindLightingUniforms(Shader *shader) {
		shader->setUniform("numDirPointSpotLights", glm::ivec4(m_DirectionalLights.size(), m_PointLights.size(), m_SpotLights.size(), 0));

		int i = 0;
		for (auto iter = m_DirectionalLights.begin(); iter != m_DirectionalLights.end(); iter++) {
			iter->setupUniforms(shader, i++);
		}

		i = 0;
		for (auto iter = m_PointLights.begin(); iter != m_PointLights.end(); iter++) {
			iter->setupUniforms(shader, i++);
		}

		i = 0;
		for (auto iter = m_SpotLights.begin(); iter != m_SpotLights.end(); iter++) {
			iter->setupUniforms(shader, i++);
		}
	}

	void DynamicLightManager::bindStaticLightingUniforms(Shader *shader) {
		int numStaticDirLights = 0;
		for (auto iter = m_DirectionalLights.begin(); iter != m_DirectionalLights.end(); iter++) {
			if (iter->m_IsStatic) 
				iter->setupUniforms(shader, numStaticDirLights++);
		}

		int numStaticPointLights = 0;
		for (auto iter = m_PointLights.begin(); iter != m_PointLights.end(); iter++) {
			if (iter->m_IsStatic)
				iter->setupUniforms(shader, numStaticPointLights++);
		}

		int numStaticSpotLights = 0;
		for (auto iter = m_SpotLights.begin(); iter != m_SpotLights.end(); iter++) {
			if (iter->m_IsStatic) 
				iter->setupUniforms(shader, numStaticSpotLights++);
		}

		shader->setUniform("numDirPointSpotLights", glm::ivec4(numStaticDirLights, numStaticPointLights, numStaticSpotLights, 0));
	}


	void DynamicLightManager::addDirectionalLight(DirectionalLight &directionalLight) {
		m_DirectionalLights.push_back(directionalLight);
	}

	void DynamicLightManager::addPointLight(PointLight &pointLight) {
		m_PointLights.push_back(pointLight);
	}

	void DynamicLightManager::addSpotLight(SpotLight &spotLight) {
		m_SpotLights.push_back(spotLight);
	}


	// Setters
	void DynamicLightManager::setDirectionalLightDirection(unsigned int index, const glm::vec3 &dir) {
#if DEBUG_ENABLED
		if (m_DirectionalLights[index].m_IsStatic)
			Logger::getInstance().warning("logged_files/warnings.txt", "DynamicLightManager Static Light Warning", "modifying directional light's direction, even though it is a static light");
#endif
		m_DirectionalLights[index].m_Direction = dir;
	}

	void DynamicLightManager::setPointLightPosition(unsigned int index, const glm::vec3 &pos) {
#if DEBUG_ENABLED
		if (m_PointLights[index].m_IsStatic)
			Logger::getInstance().warning("logged_files/warnings.txt", "DynamicLightManager Static Light Warning", "modifying point light's position, even though it is a static light");
#endif
		m_PointLights[index].m_Position = pos;
	}

	void DynamicLightManager::setSpotLightPosition(unsigned int index, const glm::vec3 &pos) {
#if DEBUG_ENABLED
		if (m_PointLights[index].m_IsStatic)
			Logger::getInstance().warning("logged_files/warnings.txt", "DynamicLightManager Static Light Warning", "modifying spot light's position, even though it is a static light");
#endif
		m_SpotLights[index].m_Position = pos;
	}
	void DynamicLightManager::setSpotLightDirection(unsigned int index, const glm::vec3 &dir) {
#if DEBUG_ENABLED
		if (m_PointLights[index].m_IsStatic)
			Logger::getInstance().warning("logged_files/warnings.txt", "DynamicLightManager Static Light Warning", "modifying spot light's direction, even though it is a static light");
#endif
		m_SpotLights[index].m_Direction = dir;
	}


	// Getters
	const glm::vec3& DynamicLightManager::getDirectionalLightDirection(unsigned int index) {
		return m_DirectionalLights[index].m_Direction;
	}

}
