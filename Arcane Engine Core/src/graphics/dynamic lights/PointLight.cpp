#include "PointLight.h"

namespace arcane { namespace graphics {

	PointLight::PointLight(glm::vec3 &amb, glm::vec3 &diff, glm::vec3 &spec, glm::vec3 &pos, float cons, float lin, float quad)
		: DynamicLight(amb, diff, spec), position(pos), constant(cons), linear(lin), quadratic(quad) {}

	// TODO: Assert that the shader is bound in debug
	void PointLight::setupUniforms(Shader &shader, int currentLightIndex) {
		if (isActive) {
			shader.setUniform3f(("pointLights[" + std::to_string(currentLightIndex) + "].ambient").c_str(), ambient);
			shader.setUniform3f(("pointLights[" + std::to_string(currentLightIndex) + "].diffuse").c_str(), diffuse);
			shader.setUniform3f(("pointLights[" + std::to_string(currentLightIndex) + "].specular").c_str(), specular);
			shader.setUniform3f(("pointLights[" + std::to_string(currentLightIndex) + "].position").c_str(), position);
			shader.setUniform1f(("pointLights[" + std::to_string(currentLightIndex) + "].constant").c_str(), constant);
			shader.setUniform1f(("pointLights[" + std::to_string(currentLightIndex) + "].linear").c_str(), linear);
			shader.setUniform1f(("pointLights[" + std::to_string(currentLightIndex) + "].quadratic").c_str(), quadratic);
		}
	}

} }