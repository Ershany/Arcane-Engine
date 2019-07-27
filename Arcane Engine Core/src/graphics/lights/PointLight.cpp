#include "pch.h"
#include "PointLight.h"

namespace arcane {

	PointLight::PointLight(glm::vec3 &lightColour, glm::vec3 &pos)
		: Light(lightColour), m_Position(pos) {}

	void PointLight::setupUniforms(Shader *shader, int currentLightIndex) {
		shader->setUniform(("pointLights[" + std::to_string(currentLightIndex) + "].position").c_str(), m_Position);
		shader->setUniform(("pointLights[" + std::to_string(currentLightIndex) + "].lightColour").c_str(), m_LightColour);
	}

}
