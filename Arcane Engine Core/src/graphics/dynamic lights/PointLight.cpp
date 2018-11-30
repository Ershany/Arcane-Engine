#include "pch.h"
#include "PointLight.h"

namespace arcane { namespace graphics {

	PointLight::PointLight(glm::vec3 &lightColour, glm::vec3 &pos)
		: DynamicLight(lightColour), position(pos) {}

	// TODO: Assert that the shader is bound in debug
	void PointLight::setupUniforms(Shader &shader, int currentLightIndex) {
		if (isActive) {
			shader.setUniform3f(("pointLights[" + std::to_string(currentLightIndex) + "].position").c_str(), position);
			shader.setUniform3f(("pointLights[" + std::to_string(currentLightIndex) + "].lightColour").c_str(), lightColour);
		}
	}

} }