#pragma once

#include "DynamicLight.h"

namespace arcane { namespace graphics {

	struct SpotLight : public DynamicLight {
	public:
		SpotLight(glm::vec3 &lightColour, glm::vec3 &pos, glm::vec3 &dir, float cutOffAngle, float outerCutOffAngle);
	
		virtual void setupUniforms(Shader &shader, int currentLightIndex) override;


		glm::vec3 position, direction;
		float cutOff, outerCutOff;
	};

} }