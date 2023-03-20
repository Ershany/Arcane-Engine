#include "arcpch.h"
#include "LightManager.h"

#include <Arcane/Graphics/Shader.h>

namespace Arcane
{
	LightManager::LightManager() {
		Init();
	}

	void LightManager::Init() {
		// Setup some lights for the scene
		DirectionalLight directionalLight1(1.0f, glm::vec3(3.25f, 3.25f, 3.25f), glm::vec3(-0.1f, -1.0f, -0.1f));
		directionalLight1.m_IsStatic = true;
		AddDirectionalLight(directionalLight1);

		SpotLight spotlight1(150.0f, glm::vec3(1.0f, 1.0f, 1.0f), 50.0f, glm::vec3(-86.9f, -5.0f, -28.2f), glm::vec3(0.0f, -1.0f, 0.0f), glm::cos(glm::radians(25.0f)), glm::cos(glm::radians(27.5f)));
		spotlight1.m_IsStatic = false;
		AddSpotLight(spotlight1);

		PointLight pointLight1(10.0f, glm::vec3(0.0f, 1.0f, 0.0f), 30.0f, glm::vec3(24.1f, 2.2f, 47.5f));
		pointLight1.m_IsStatic = false;
		AddPointLight(pointLight1);

		PointLight pointLight2(30.0f, glm::vec3(1.0f, 0.0f, 1.0f), 30.0f, glm::vec3(-27.2f, -9.0f, 52.0f));
		pointLight2.m_IsStatic = true;
		AddPointLight(pointLight2);
	}

	void LightManager::BindLightingUniforms(Shader *shader) {
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

	void LightManager::BindStaticLightingUniforms(Shader *shader) {
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


	void LightManager::AddDirectionalLight(DirectionalLight &directionalLight) {
		m_DirectionalLights.push_back(directionalLight);
	}

	void LightManager::AddPointLight(PointLight &pointLight) {
		m_PointLights.push_back(pointLight);
	}

	void LightManager::AddSpotLight(SpotLight &spotLight) {
		m_SpotLights.push_back(spotLight);
	}


	// Setters
	void LightManager::SetDirectionalLightDirection(unsigned int index, const glm::vec3 &dir) {
#ifdef ARC_DEV_BUILD
		if (m_DirectionalLights[index].m_IsStatic)
			ARC_LOG_WARN("Attempting to modify directional light at index {0}, even though it is set as a static light", index);
#endif // ARC_DEV_BUILD
		m_DirectionalLights[index].m_Direction = dir;
	}

	void LightManager::SetPointLightPosition(unsigned int index, const glm::vec3 &pos) {
#ifdef ARC_DEV_BUILD
		if (m_PointLights[index].m_IsStatic)
			ARC_LOG_WARN("Attempting to modify point light position at index {0}, even though it is set as a static light", index);
#endif // ARC_DEV_BUILD
		m_PointLights[index].m_Position = pos;
	}

	void LightManager::SetSpotLightPosition(unsigned int index, const glm::vec3 &pos) {
#ifdef ARC_DEV_BUILD
		if (m_PointLights[index].m_IsStatic)
			ARC_LOG_WARN("Attempting to modify spot light position at index {0}, even though it is set as a static light", index);
#endif // ARC_DEV_BUILD
		m_SpotLights[index].m_Position = pos;
	}
	void LightManager::SetSpotLightDirection(unsigned int index, const glm::vec3 &dir) {
#ifdef ARC_DEV_BUILD
		if (m_PointLights[index].m_IsStatic)
			ARC_LOG_WARN("Attempting to modify spot light direction at index {0}, even though it is set as a static light", index);
#endif // ARC_DEV_BUILD
		m_SpotLights[index].m_Direction = dir;
	}


	// Getters
	const glm::vec3& LightManager::GetDirectionalLightDirection(unsigned int index) {
		return m_DirectionalLights[index].m_Direction;
	}
}
