#pragma once

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

namespace arcane {

	class DynamicLightManager {
	public:
		DynamicLightManager();

		void bindLightingUniforms(Shader *shader);
		void bindStaticLightingUniforms(Shader *shader);

		void addDirectionalLight(DirectionalLight &directionalLight);
		void addPointLight(PointLight &pointLight);
		void addSpotLight(SpotLight &spotLight);

		// Control functions for directional lights
		void setDirectionalLightDirection(unsigned int index, const glm::vec3 &dir);

		// Control functions for point lights
		void setPointLightPosition(unsigned int index, const glm::vec3 &pos);

		// Control functions for spot lights
		void setSpotLightPosition(unsigned int index, const glm::vec3 &pos);
		void setSpotLightDirection(unsigned int index, const glm::vec3 &dir);


		// Getters
		const glm::vec3& getDirectionalLightDirection(unsigned int index);
	private:
		void init();
		
		std::vector<DirectionalLight> m_DirectionalLights;
		std::vector<PointLight> m_PointLights;
		std::vector<SpotLight> m_SpotLights;
	};

}
