#pragma once

#include <Arcane/Graphics/Lights/DirectionalLight.h>
#include <Arcane/Graphics/Lights/PointLight.h>
#include <Arcane/Graphics/Lights/SpotLight.h>

namespace Arcane
{
	class Shader;

	enum class LightType : int
	{
		LightType_Directional,
		LightType_Point,
		LightType_Spot,
		LightTypeSize
	};

	enum class ShadowQuality : int
	{
		ShadowQuality_Low,			// 256 x 256
		ShadowQuality_Medium,		// 512 x 512
		ShadowQuality_High,			// 1024 x 1024
		ShadowQuality_Ultra,		// 2048 x 2048
		ShadowQuality_Nightmare,	// 4096 x 4096
		ShadowQualitySize
	};

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
