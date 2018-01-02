#pragma once

#include "DynamicLight.h"

namespace arcane { namespace graphics {

	struct DirectionalLight : public DynamicLight {
		DirectionalLight(glm::vec3 &amb, glm::vec3 &diff, glm::vec3 &spec, glm::vec3 &dir);

		virtual void setupUniforms(Shader &shader, int currentLightIndex) override;


		glm::vec3 direction;
	};

} }