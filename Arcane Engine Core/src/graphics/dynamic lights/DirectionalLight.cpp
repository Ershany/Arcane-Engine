#include "pch.h"
#include "DirectionalLight.h"

namespace arcane { namespace graphics {

	DirectionalLight::DirectionalLight(glm::vec3 &lightColour, glm::vec3 &dir)
		: DynamicLight(lightColour), direction(dir) {}

	// TODO: Add in multiple directional light support
	// TODO: Assert that the shader is bound in debug
	void DirectionalLight::setupUniforms(Shader &shader, int currentLightIndex) {
		if (isActive) {
			shader.setUniform3f("dirLight.direction", direction);
			shader.setUniform3f("dirLight.lightColour", lightColour);
		}
	}

} }