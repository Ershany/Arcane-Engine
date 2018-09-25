#pragma once

#include "DynamicLight.h"

namespace arcane { namespace graphics {

	struct DirectionalLight : public DynamicLight {
		DirectionalLight();
		DirectionalLight(glm::vec3 &lightColour, glm::vec3 &dir);

		virtual void setupUniforms(Shader &shader, int currentLightIndex) override;


		glm::vec3 direction;
	};

} }