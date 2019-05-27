#pragma once

#include "Light.h"

namespace arcane {

	class DynamicLightManager;

	class PointLight : public Light {
		friend DynamicLightManager;
	public:
		PointLight(glm::vec3 &lightColour, glm::vec3 &pos);

		virtual void setupUniforms(Shader *shader, int currentLightIndex) override;
	private:
		glm::vec3 m_Position;
	};

}
