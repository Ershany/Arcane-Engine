#pragma once

namespace Arcane
{
	class Scene;
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

	class LightManager {
	public:
		LightManager(Scene *scene);

		void BindLightingUniforms(Shader *shader);
		void BindStaticLightingUniforms(Shader *shader);
		glm::vec2 GetShadowQualityResolution(ShadowQuality quality);

		// Getters
		glm::vec3 GetDirectionalLightDirection(unsigned int index);
	private:
		void Init();

		void BindLights(Shader *shader, bool bindOnlyStatic);
	private:
		Scene *m_Scene;
	};
}
