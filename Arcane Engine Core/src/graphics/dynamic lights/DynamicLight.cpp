#include "DynamicLight.h"

namespace arcane { namespace graphics {

	DynamicLight::DynamicLight(glm::vec3 &ambient, glm::vec3 &diffuse, glm::vec3 &specular) 
		: m_Ambient(ambient), m_Diffuse(diffuse), m_Specular(specular) {}

} }
