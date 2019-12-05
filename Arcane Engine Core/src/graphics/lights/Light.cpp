#include "pch.h"
#include "Light.h"

namespace arcane {

	Light::Light(float lightIntensity, glm::vec3 &lightColour) 
		: m_Intensity(lightIntensity), m_LightColour(lightColour), m_IsStatic(false) {}

}
