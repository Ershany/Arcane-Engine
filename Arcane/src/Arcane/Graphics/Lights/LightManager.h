#pragma once

namespace Arcane
{
	class Framebuffer;
	struct LightComponent;
	struct TransformComponent;
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

	class LightManager
	{
	public:
		LightManager(Scene *scene);

		void Init();
		void Update();

		void BindLightingUniforms(Shader *shader);
		void BindStaticLightingUniforms(Shader *shader);

		static glm::uvec2 GetShadowQualityResolution(ShadowQuality quality);

		// Getters
		inline bool HasDirectionalShadowCaster() const { return m_ClosestDirectionalShadowCaster != nullptr; }
		Framebuffer *GetDirectionalShadowCasterFramebuffer() { return m_DirectionalShadowFramebuffer; }
		glm::vec3 GetDirectionalShadowCasterLightDir();
		glm::vec2 GetDirectionalShadowCasterNearFarPlane();
		float GetDirectionalShadowCasterBias();
	private:
		void FindClosestDirectionalLightShadowCaster();
		void BindLights(Shader *shader, bool bindOnlyStatic);
		void ReallocateTargets(glm::uvec2 newResolution);
	private:
		Scene *m_Scene;

		LightComponent *m_ClosestDirectionalShadowCaster;
		TransformComponent *m_ClosestDirectionalShadowCasterTransform;
		Framebuffer *m_DirectionalShadowFramebuffer;
	};
}
