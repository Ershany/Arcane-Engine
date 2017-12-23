#include "SpotLight.h"

namespace arcane { namespace graphics {

	SpotLight::SpotLight(glm::vec3 &ambient, glm::vec3 &diffuse, glm::vec3 &specular, glm::vec3 &position, glm::vec3 &direction, float cutOff, float outerCutOff, float constant, float linear, float quadratic) 
		: DynamicLight(ambient, diffuse, specular), m_Position(position), m_Direction(direction), m_CutOff(cutOff), m_OuterCutOff(outerCutOff), m_Constant(constant), m_Linear(linear), m_Quadratic(quadratic) {}

	// TODO: Add in multiple spotlight support
	void SpotLight::setupUniforms(Shader &shader, int currentLightIndex) {
		shader.setUniform3f("spotLight.ambient", m_Ambient);
		shader.setUniform3f("spotLight.diffuse", m_Diffuse);
		shader.setUniform3f("spotLight.specular", m_Specular);
		shader.setUniform3f("spotLight.position", m_Position);
		shader.setUniform3f("spotLight.direction", m_Direction);
		shader.setUniform1f("spotLight.cutOff", m_CutOff);
		shader.setUniform1f("spotLight.outerCutOff", m_OuterCutOff);
		shader.setUniform1f("spotLight.constant", m_Constant);
		shader.setUniform1f("spotLight.linear", m_Linear);
		shader.setUniform1f("spotLight.quadratic", m_Quadratic);
	}

} }