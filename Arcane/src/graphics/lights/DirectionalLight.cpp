#include "arcpch.h"
#include "DirectionalLight.h"

namespace Arcane 
{
	DirectionalLight::DirectionalLight(float lightIntensity, glm::vec3 &lightColour, glm::vec3 &dir)
		: Light(lightIntensity, lightColour), m_Direction(dir) {}

	void DirectionalLight::SetupUniforms(Shader *shader, int currentLightIndex) {
		shader->SetUniform(("dirLights[" + std::to_string(currentLightIndex) + "].direction").c_str(), m_Direction);
		shader->SetUniform(("dirLights[" + std::to_string(currentLightIndex) + "].intensity").c_str(), m_Intensity);
		shader->SetUniform(("dirLights[" + std::to_string(currentLightIndex) + "].lightColour").c_str(), m_LightColour);
	}
}
