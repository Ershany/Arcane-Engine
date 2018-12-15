#pragma once

#include "DynamicLight.h"

namespace arcane {

	struct DirectionalLight : public DynamicLight {
		DirectionalLight(glm::vec3 &lightColour, glm::vec3 &dir);

		virtual void setupUniforms(Shader &shader, int currentLightIndex) override;


		glm::vec3 direction;
	};

}
