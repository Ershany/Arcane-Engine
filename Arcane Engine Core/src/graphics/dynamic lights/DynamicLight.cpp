#include "DynamicLight.h"

namespace arcane { namespace graphics {

	DynamicLight::DynamicLight(glm::vec3 &amb, glm::vec3 &diff, glm::vec3 &spec) 
		: ambient(amb), diffuse(diff), specular(spec), isActive(false) {}

} }
