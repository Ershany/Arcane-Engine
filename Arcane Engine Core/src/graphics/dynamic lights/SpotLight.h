#pragma once

#include "DynamicLight.h"

namespace arcane { namespace graphics {

	class SpotLight : public DynamicLight {
	public:
		SpotLight(glm::vec3 &ambient, glm::vec3 &diffuse, glm::vec3 &specular, glm::vec3 &position, glm::vec3 &direction, float cutOff, float outerCutOff, float constant, float linear, float quadratic);
	
		virtual void setupUniforms(Shader &shader, int currentLightIndex) override;
	private:
		glm::vec3 m_Position, m_Direction;
		float m_CutOff, m_OuterCutOff;
		float m_Constant, m_Linear, m_Quadratic;
	};

} }