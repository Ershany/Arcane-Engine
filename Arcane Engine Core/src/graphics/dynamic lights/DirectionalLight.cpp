#include "DirectionalLight.h"

namespace arcane { namespace graphics {

	DirectionalLight::DirectionalLight(glm::vec3 &amb, glm::vec3 &diff, glm::vec3 &spec, glm::vec3 &dir)
		: DynamicLight(amb, diff, spec), direction(dir) {}

	// TODO: Add in multiple directional light support
	// TODO: Assert that the shader is bound in debug
	void DirectionalLight::setupUniforms(Shader &shader, int currentLightIndex) {
		if (isActive) {
			shader.setUniform3f("dirLight.ambient", ambient);
			shader.setUniform3f("dirLight.diffuse", diffuse);
			shader.setUniform3f("dirLight.specular", specular);
			shader.setUniform3f("dirLight.direction", direction);
		}
	}

} }