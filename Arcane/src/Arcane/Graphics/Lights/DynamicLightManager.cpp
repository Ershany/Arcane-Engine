#include "arcpch.h"
#include "DynamicLightManager.h"

#include <Arcane/Graphics/Shader.h>

namespace Arcane
{
	DynamicLightManager::DynamicLightManager() {
		Init();
	}

	void DynamicLightManager::Init() {
		// Setup some lights for the scene
		DirectionalLight directionalLight1(2.0f, glm::vec3(3.25f, 3.25f, 3.25f), glm::vec3(-0.1f, -1.0f, -0.1f));
		directionalLight1.m_IsStatic = true;
		AddDirectionalLight(directionalLight1);

		SpotLight spotlight1(100.0f, glm::vec3(1.0f, 1.0f, 1.0f), 50.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));
		spotlight1.m_IsStatic = false;
		AddSpotLight(spotlight1);

		PointLight pointLight1(10.0f, glm::vec3(1.0f, 0.0f, 1.0f), 30.0f, glm::vec3(120.0f, 84.0f, 122.0f));
		pointLight1.m_IsStatic = false;
		AddPointLight(pointLight1);

		PointLight pointLight2(10.0f, glm::vec3(1.0f, 0.25f, 0.0f), 30.0f, glm::vec3(120.0f, 77.0f, 100.0f));
		pointLight2.m_IsStatic = true;
		AddPointLight(pointLight2);
	}

	void DynamicLightManager::BindLightingUniforms(Shader *shader) {
		shader->SetUniform("numDirPointSpotLights", glm::ivec4(m_DirectionalLights.size(), m_PointLights.size(), m_SpotLights.size(), 0));

		int i = 0;
		for (auto iter = m_DirectionalLights.begin(); iter != m_DirectionalLights.end(); iter++) {
			iter->SetupUniforms(shader, i++);
		}

		i = 0;
		for (auto iter = m_PointLights.begin(); iter != m_PointLights.end(); iter++) {
			iter->SetupUniforms(shader, i++);
		}

		i = 0;
		for (auto iter = m_SpotLights.begin(); iter != m_SpotLights.end(); iter++) {
			iter->SetupUniforms(shader, i++);
		}
	}

	void DynamicLightManager::BindStaticLightingUniforms(Shader *shader) {
		int numStaticDirLights = 0;
		for (auto iter = m_DirectionalLights.begin(); iter != m_DirectionalLights.end(); iter++) {
			if (iter->m_IsStatic) 
				iter->SetupUniforms(shader, numStaticDirLights++);
		}

		int numStaticPointLights = 0;
		for (auto iter = m_PointLights.begin(); iter != m_PointLights.end(); iter++) {
			if (iter->m_IsStatic)
				iter->SetupUniforms(shader, numStaticPointLights++);
		}

		int numStaticSpotLights = 0;
		for (auto iter = m_SpotLights.begin(); iter != m_SpotLights.end(); iter++) {
			if (iter->m_IsStatic) 
				iter->SetupUniforms(shader, numStaticSpotLights++);
		}

		shader->SetUniform("numDirPointSpotLights", glm::ivec4(numStaticDirLights, numStaticPointLights, numStaticSpotLights, 0));
	}


	void DynamicLightManager::AddDirectionalLight(DirectionalLight &directionalLight) {
		m_DirectionalLights.push_back(directionalLight);
	}

	void DynamicLightManager::AddPointLight(PointLight &pointLight) {
		m_PointLights.push_back(pointLight);
	}

	void DynamicLightManager::AddSpotLight(SpotLight &spotLight) {
		m_SpotLights.push_back(spotLight);
	}


	// Setters
	void DynamicLightManager::SetDirectionalLightDirection(unsigned int index, const glm::vec3 &dir) {
#ifdef ARC_DEV_BUILD
		if (m_DirectionalLights[index].m_IsStatic)
			ARC_LOG_WARN("Attempting to modify directional light at index {0}, even though it is set as a static light", index);
#endif // ARC_DEV_BUILD
		m_DirectionalLights[index].m_Direction = dir;
	}

	void DynamicLightManager::SetPointLightPosition(unsigned int index, const glm::vec3 &pos) {
#ifdef ARC_DEV_BUILD
		if (m_PointLights[index].m_IsStatic)
			ARC_LOG_WARN("Attempting to modify point light position at index {0}, even though it is set as a static light", index);
#endif // ARC_DEV_BUILD
		m_PointLights[index].m_Position = pos;
	}

	void DynamicLightManager::SetSpotLightPosition(unsigned int index, const glm::vec3 &pos) {
#ifdef ARC_DEV_BUILD
		if (m_PointLights[index].m_IsStatic)
			ARC_LOG_WARN("Attempting to modify spot light position at index {0}, even though it is set as a static light", index);
#endif // ARC_DEV_BUILD
		m_SpotLights[index].m_Position = pos;
	}
	void DynamicLightManager::SetSpotLightDirection(unsigned int index, const glm::vec3 &dir) {
#ifdef ARC_DEV_BUILD
		if (m_PointLights[index].m_IsStatic)
			ARC_LOG_WARN("Attempting to modify spot light direction at index {0}, even though it is set as a static light", index);
#endif // ARC_DEV_BUILD
		m_SpotLights[index].m_Direction = dir;
	}


	// Getters
	const glm::vec3& DynamicLightManager::GetDirectionalLightDirection(unsigned int index) {
		return m_DirectionalLights[index].m_Direction;
	}
}
