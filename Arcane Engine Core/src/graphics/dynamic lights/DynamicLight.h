#pragma once

#include <graphics/Shader.h>

namespace arcane { namespace graphics {

	struct DynamicLight {
		DynamicLight(glm::vec3 &lightColour);

		virtual void setupUniforms(Shader &shader, int currentLightIndex) = 0;


		glm::vec3 lightColour;
		bool isActive;
	};

} }
