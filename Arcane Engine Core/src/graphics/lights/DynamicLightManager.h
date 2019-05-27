#pragma once

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

namespace arcane {

	class DynamicLightManager {
	public:
		DynamicLightManager();

		void setupLightingUniforms(Shader *shader);
		void addSpotLight(SpotLight &spotLight);
		void addPointLight(PointLight &pointLight);

		// Control functions
		inline void setSpotLightPosition(int index, const glm::vec3 &pos) { m_SpotLights[index].m_Position = pos; }
		inline void setSpotLightDirection(int index, const glm::vec3 &dir) { m_SpotLights[index].m_Direction = dir; }

		inline void setDirectionalLightDirection(const glm::vec3 &dir) { m_DirectionalLight.m_Direction = dir; }

		inline void setPointLightPosition(int index, const glm::vec3 &pos) { m_PointLights[index].m_Position = pos; }

		// Getters
		inline const glm::vec3& getDirectionalLightDirection() { return m_DirectionalLight.m_Direction; }
	private:
		void init();
		
		DirectionalLight m_DirectionalLight;
		std::vector<SpotLight> m_SpotLights;
		std::vector<PointLight> m_PointLights;
	};

}
