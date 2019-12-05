#include "pch.h"
#include "DirectionalLight.h"

namespace arcane {

	DirectionalLight::DirectionalLight(float lightIntensity, glm::vec3 &lightColour, glm::vec3 &dir)
		: Light(lightIntensity, lightColour), m_Direction(dir) {}

	void DirectionalLight::setupUniforms(Shader *shader, int currentLightIndex) {
		shader->setUniform(("dirLights[" + std::to_string(currentLightIndex) + "].direction").c_str(), m_Direction);
		shader->setUniform(("dirLights[" + std::to_string(currentLightIndex) + "].intensity").c_str(), m_Intensity);
		shader->setUniform(("dirLights[" + std::to_string(currentLightIndex) + "].lightColour").c_str(), m_LightColour);
	}

}
