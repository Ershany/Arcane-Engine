#include "pch.h"
#include "SpotLight.h"

namespace arcane {

	SpotLight::SpotLight(glm::vec3 &lightColour, glm::vec3 &pos, glm::vec3 &dir, float cutOffAngle, float outerCutOffAngle)
		: Light(lightColour), m_Position(pos), m_Direction(dir), m_CutOff(cutOffAngle), m_OuterCutOff(outerCutOffAngle) {}

	void SpotLight::setupUniforms(Shader *shader, int currentLightIndex) {
		shader->setUniform(("spotLights[" + std::to_string(currentLightIndex) + "].position").c_str(), m_Position);
		shader->setUniform(("spotLights[" + std::to_string(currentLightIndex) + "].direction").c_str(), m_Direction);
		shader->setUniform(("spotLights[" + std::to_string(currentLightIndex) + "].lightColour").c_str(), m_LightColour);
		shader->setUniform(("spotLights[" + std::to_string(currentLightIndex) + "].cutOff").c_str(), m_CutOff);
		shader->setUniform(("spotLights[" + std::to_string(currentLightIndex) + "].outerCutOff").c_str(), m_OuterCutOff);
	}

}
