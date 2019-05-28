#include "pch.h"
#include "Light.h"

namespace arcane {

	Light::Light(glm::vec3 &lightColour) 
		: m_LightColour(lightColour), m_IsStatic(false) {}

}
