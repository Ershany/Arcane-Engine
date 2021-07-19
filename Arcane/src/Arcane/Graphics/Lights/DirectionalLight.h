#pragma once

#include <Arcane/Graphics/Lights/Light.h>

namespace Arcane
{
	
	class DynamicLightManager;

	class DirectionalLight : public Light {
		friend DynamicLightManager;
	public:
		DirectionalLight(float lightIntensity, glm::vec3 &lightColour, glm::vec3 &dir);

		virtual void SetupUniforms(Shader *shader, int currentLightIndex) override;
	private:
		glm::vec3 m_Direction;
	};
}
