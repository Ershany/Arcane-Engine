#pragma once

#include "DynamicLight.h"

#include <string>

namespace arcane { namespace graphics {

	class PointLight : public DynamicLight {
	public:
		PointLight(glm::vec3 &ambient, glm::vec3 &diffuse, glm::vec3 &specular, glm::vec3 &position, float constant, float linear, float quadratic);

		virtual void setupUniforms(Shader &shader, int currentLightIndex) override;
	private:
		glm::vec3 m_Position;
		float m_Constant, m_Linear, m_Quadratic;
	};

} }