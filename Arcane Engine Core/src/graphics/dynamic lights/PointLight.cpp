#include "PointLight.h"

namespace arcane { namespace graphics {

	PointLight::PointLight(glm::vec3 &ambient, glm::vec3 &diffuse, glm::vec3 &specular, glm::vec3 &position, float constant, float linear, float quadratic)
		: DynamicLight(ambient, diffuse, specular), m_Position(position), m_Constant(constant), m_Linear(linear), m_Quadratic(quadratic) {}

	void PointLight::setupUniforms(Shader &shader, int currentLightIndex) {
		shader.setUniform3f(("pointLights[" + std::to_string(currentLightIndex) + "].ambient").c_str(), m_Ambient);
		shader.setUniform3f(("pointLights[" + std::to_string(currentLightIndex) + "].diffuse").c_str(), m_Diffuse);
		shader.setUniform3f(("pointLights[" + std::to_string(currentLightIndex) + "].specular").c_str(), m_Specular);
		shader.setUniform3f(("pointLights[" + std::to_string(currentLightIndex) + "].position").c_str(), m_Position);
		shader.setUniform1f(("pointLights[" + std::to_string(currentLightIndex) + "].constant").c_str(), m_Constant);
		shader.setUniform1f(("pointLights[" + std::to_string(currentLightIndex) + "].linear").c_str(), m_Linear);
		shader.setUniform1f(("pointLights[" + std::to_string(currentLightIndex) + "].quadratic").c_str(), m_Quadratic);
	}

} }