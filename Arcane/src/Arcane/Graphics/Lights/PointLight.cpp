#include "arcpch.h"
#include "PointLight.h"

#include <Arcane/Graphics/Shader.h>

namespace Arcane
{
	PointLight::PointLight(float lightIntensity, glm::vec3 &lightColour, float attenuationRadius, glm::vec3 &pos)
		: Light(lightIntensity, lightColour), m_AttenuationRadius(attenuationRadius), m_Position(pos) {}

	void PointLight::SetupUniforms(Shader *shader, int currentLightIndex) {
		shader->SetUniform(("pointLights[" + std::to_string(currentLightIndex) + "].position").c_str(), m_Position);
		shader->SetUniform(("pointLights[" + std::to_string(currentLightIndex) + "].intensity").c_str(), m_Intensity);
		shader->SetUniform(("pointLights[" + std::to_string(currentLightIndex) + "].lightColour").c_str(), m_LightColour);
		shader->SetUniform(("pointLights[" + std::to_string(currentLightIndex) + "].attenuationRadius").c_str(), m_AttenuationRadius);
	}
}
