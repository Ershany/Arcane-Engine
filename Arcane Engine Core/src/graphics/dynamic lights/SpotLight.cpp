#include "SpotLight.h"

namespace arcane { namespace graphics {

	SpotLight::SpotLight(glm::vec3 &amb, glm::vec3 &diff, glm::vec3 &spec, glm::vec3 &pos, glm::vec3 &dir, float cutOffAngle, float outerCutOffAngle, float cons, float lin, float quad)
		: DynamicLight(amb, diff, spec), position(pos), direction(dir), cutOff(cutOffAngle), outerCutOff(outerCutOffAngle), constant(cons), linear(lin), quadratic(quad) {}

	// TODO: Add in multiple spotlight support
	// TODO: Assert that the shader is bound in debug
	void SpotLight::setupUniforms(Shader &shader, int currentLightIndex) {
		if (isActive) {
			shader.setUniform3f("spotLight.ambient", ambient);
			shader.setUniform3f("spotLight.diffuse", diffuse);
			shader.setUniform3f("spotLight.specular", specular);
			shader.setUniform3f("spotLight.position", position);
			shader.setUniform3f("spotLight.direction", direction);
			shader.setUniform1f("spotLight.cutOff", cutOff);
			shader.setUniform1f("spotLight.outerCutOff", outerCutOff);
			shader.setUniform1f("spotLight.constant", constant);
			shader.setUniform1f("spotLight.linear", linear);
			shader.setUniform1f("spotLight.quadratic", quadratic);
		}
	}

} }