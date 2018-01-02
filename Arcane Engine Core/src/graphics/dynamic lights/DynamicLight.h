#pragma once

#include <glm\common.hpp>

#include "../Shader.h"

namespace arcane { namespace graphics {

	// TODO: Add functionality so it can update with an entitie's position and orientation
	struct DynamicLight {
		DynamicLight(glm::vec3 &amb, glm::vec3 &diff, glm::vec3 &spec);

		virtual void setupUniforms(Shader &shader, int currentLightIndex) = 0;


		glm::vec3 ambient, diffuse, specular;
		bool isActive;
	};

} }
