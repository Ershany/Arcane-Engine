#pragma once

#include <glm\common.hpp>

#include "../Shader.h"

namespace arcane { namespace graphics {

	class DynamicLight {
	protected:
		DynamicLight(glm::vec3 &ambient, glm::vec3 &diffuse, glm::vec3 &specular);
	public:
		virtual void setupUniforms(Shader &shader, int currentLightIndex) = 0;
	protected:
		glm::vec3 m_Ambient, m_Diffuse, m_Specular;
	};

} }
