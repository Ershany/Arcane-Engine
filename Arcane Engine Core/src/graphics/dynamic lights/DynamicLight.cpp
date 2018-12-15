#include "pch.h"
#include "DynamicLight.h"

namespace arcane {

	DynamicLight::DynamicLight(glm::vec3 &lightColour) 
		: lightColour(lightColour), isActive(false) {}

}
