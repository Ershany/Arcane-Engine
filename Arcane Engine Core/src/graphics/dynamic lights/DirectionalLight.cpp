#include "DirectionalLight.h"

namespace arcane { namespace graphics {

	DirectionalLight::DirectionalLight(glm::vec3 &ambient, glm::vec3 &diffuse, glm::vec3 &specular, glm::vec3 &direction)
		: DynamicLight(ambient, diffuse, specular), m_Direction(direction) {}

	// TODO: Add in multiple directional light support
	void DirectionalLight::setupUniforms(Shader &shader, int currentLightIndex) {
		shader.setUniform3f("dirLight.ambient", m_Ambient);
		shader.setUniform3f("dirLight.diffuse", m_Diffuse);
		shader.setUniform3f("dirLight.specular", m_Specular);
		shader.setUniform3f("dirLight.direction", m_Direction);
	}

} }