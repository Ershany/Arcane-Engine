#pragma once

#include <Arcane/Graphics/Lights/Light.h>

namespace Arcane
{
	class LightManager;

	class PointLight : public Light {
		friend LightManager;
	public:
		PointLight(float lightIntensity, glm::vec3 &lightColour, float attenuationRadius, glm::vec3 &pos);

		virtual void SetupUniforms(Shader *shader, int currentLightIndex) override;
	private:
		float m_AttenuationRadius;
		glm::vec3 m_Position;
	};
}
