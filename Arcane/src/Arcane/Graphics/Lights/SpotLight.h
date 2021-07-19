#pragma once

#include <Arcane/Graphics/Lights/Light.h>

namespace Arcane
{
	class DynamicLightManager;

	class SpotLight : public Light {
		friend DynamicLightManager;
	public:
		SpotLight(float lightIntensity, glm::vec3 &lightColour, float attenuationRadius, glm::vec3 &pos, glm::vec3 &dir, float cutOffAngle, float outerCutOffAngle);
	
		virtual void SetupUniforms(Shader *shader, int currentLightIndex) override;
	private:
		float m_AttenuationRadius;
		glm::vec3 m_Position, m_Direction;
		float m_CutOff, m_OuterCutOff;
	};
}
