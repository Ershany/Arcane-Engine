#include "arcpch.h"
#include "SpotLight.h"

namespace Arcane
{
	SpotLight::SpotLight(float lightIntensity, glm::vec3 &lightColour, float attenuationRadius, glm::vec3 &pos, glm::vec3 &dir, float cutOffAngle, float outerCutOffAngle)
		: Light(lightIntensity, lightColour), m_AttenuationRadius(attenuationRadius), m_Position(pos), m_Direction(dir), m_CutOff(cutOffAngle), m_OuterCutOff(outerCutOffAngle) {}

	void SpotLight::SetupUniforms(Shader *shader, int currentLightIndex) {
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].position").c_str(), m_Position);
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].direction").c_str(), m_Direction);
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].intensity").c_str(), m_Intensity);
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].lightColour").c_str(), m_LightColour);
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].attenuationRadius").c_str(), m_AttenuationRadius);
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].cutOff").c_str(), m_CutOff);
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].outerCutOff").c_str(), m_OuterCutOff);
	}
}
