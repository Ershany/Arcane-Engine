#pragma once

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

#include <vector>

namespace arcane { namespace graphics {

	class DynamicLightManager {
	public:
		DynamicLightManager();

		void setupLightingUniforms(Shader &shader);
		void addPointLight(PointLight &pointLight);

		// Control functions
		inline void setSpotLightPosition(glm::vec3 &pos) { m_SpotLight.position = pos; }
		inline void setSpotLightDirection(glm::vec3 &dir) { m_SpotLight.direction = dir; }
	private:
		void init();
		
		DirectionalLight m_DirectionalLight;
		SpotLight m_SpotLight;
		std::vector<PointLight> m_PointLights;
	};

} }