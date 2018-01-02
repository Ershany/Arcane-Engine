#pragma once

#include "DynamicLight.h"

namespace arcane { namespace graphics {

	struct SpotLight : public DynamicLight {
	public:
		SpotLight(glm::vec3 &amb, glm::vec3 &diff, glm::vec3 &spec, glm::vec3 &pos, glm::vec3 &dir, float cutOffAngle, float outerCutOffAngle, float cons, float lin, float quad);
	
		virtual void setupUniforms(Shader &shader, int currentLightIndex) override;


		glm::vec3 position, direction;
		float cutOff, outerCutOff;
		float constant, linear, quadratic;
	};

} }