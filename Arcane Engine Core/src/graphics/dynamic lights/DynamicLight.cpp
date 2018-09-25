#include "DynamicLight.h"

namespace arcane { namespace graphics {

	DynamicLight::DynamicLight(glm::vec3 &lightColour) 
		: lightColour(lightColour), isActive(false) {}

} }
