#pragma once

#include "Light.h"

namespace arcane {
	
	class DynamicLightManager;

	class DirectionalLight : public Light {
		friend DynamicLightManager;
	public:
		DirectionalLight(glm::vec3 &lightColour, glm::vec3 &dir);

		virtual void setupUniforms(Shader *shader, int currentLightIndex) override;
	private:
		glm::vec3 m_Direction;
	};

}
