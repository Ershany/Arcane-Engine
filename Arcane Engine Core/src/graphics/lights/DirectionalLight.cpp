#include "pch.h"
#include "DirectionalLight.h"

namespace arcane {

	DirectionalLight::DirectionalLight(glm::vec3 &lightColour, glm::vec3 &dir)
		: Light(lightColour), m_Direction(dir) {}

	void DirectionalLight::setupUniforms(Shader *shader, int currentLightIndex) {
		if (m_IsActive) {
			shader->setUniform3f(("dirLights[" + std::to_string(currentLightIndex) + "].direction").c_str(), m_Direction);
			shader->setUniform3f(("dirLights[" + std::to_string(currentLightIndex) + "].lightColour").c_str(), m_LightColour);
		}
	}

}
