#include "arcpch.h"
#include "Light.h"

#include <Arcane/Graphics/Shader.h>

namespace Arcane
{
	Light::Light(float lightIntensity, glm::vec3 &lightColour) 
		: m_Intensity(lightIntensity), m_LightColour(lightColour), m_IsStatic(false) {}
}
