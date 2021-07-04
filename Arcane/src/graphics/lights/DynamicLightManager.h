#pragma once

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

namespace Arcane
{
	class DynamicLightManager {
	public:
		DynamicLightManager();

		void BindLightingUniforms(Shader *shader);
		void BindStaticLightingUniforms(Shader *shader);

		void AddDirectionalLight(DirectionalLight &directionalLight);
		void AddPointLight(PointLight &pointLight);
		void AddSpotLight(SpotLight &spotLight);

		// Control functions for directional lights
		void SetDirectionalLightDirection(unsigned int index, const glm::vec3 &dir);

		// Control functions for point lights
		void SetPointLightPosition(unsigned int index, const glm::vec3 &pos);

		// Control functions for spot lights
		void SetSpotLightPosition(unsigned int index, const glm::vec3 &pos);
		void SetSpotLightDirection(unsigned int index, const glm::vec3 &dir);


		// Getters
		const glm::vec3& GetDirectionalLightDirection(unsigned int index);
	private:
		void Init();
		
		std::vector<DirectionalLight> m_DirectionalLights;
		std::vector<PointLight> m_PointLights;
		std::vector<SpotLight> m_SpotLights;
	};
}
