#pragma once

#include "DynamicLight.h"

namespace arcane { namespace graphics {

	struct PointLight : public DynamicLight {
	public:
		PointLight(glm::vec3 &lightColour, glm::vec3 &pos);

		virtual void setupUniforms(Shader &shader, int currentLightIndex) override;


		glm::vec3 position;
	};

} }