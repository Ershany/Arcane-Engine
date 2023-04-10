#pragma once

namespace Arcane
{
	class Framebuffer;
	class Cubemap;
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
		~LightManager();

		void Init();
		void Update();

		void BindLightingUniforms(Shader *shader);
		void BindStaticLightingUniforms(Shader *shader);

		static glm::uvec2 GetShadowQualityResolution(ShadowQuality quality);

		// Getters for directional light shadow caster
		inline bool HasDirectionalLightShadowCaster() const { return m_ClosestDirectionalLightShadowCaster != nullptr; }
		Framebuffer* GetDirectionalLightShadowCasterFramebuffer() { return m_DirectionalLightShadowFramebuffer; }
		glm::vec3 GetDirectionalLightShadowCasterLightDir();
		glm::vec2 GetDirectionalLightShadowCasterNearFarPlane();
		float GetDirectionalLightShadowCasterBias();
		int GetDirectionalLightShadowCasterIndex();

		// Getters for spot light shadow caster
		inline bool HasSpotLightShadowCaster() const { return m_ClosestSpotLightShadowCaster != nullptr; }
		Framebuffer* GetSpotLightShadowCasterFramebuffer() { return m_SpotLightShadowFramebuffer; }
		glm::vec3 GetSpotLightShadowCasterLightDir();
		glm::vec3 GetSpotLightShadowCasterLightPosition();
		float GetSpotLightShadowCasterOuterCutOffAngle(); // Returns outer cutoff angle in radians
		float GetSpotLightShadowCasterAttenuationRange();
		glm::vec2 GetSpotLightShadowCasterNearFarPlane();
		float GetSpotLightShadowCasterBias();
		int GetSpotLightShadowCasterIndex();

		// Getters for point light shadow caster
		inline bool HasPointlightShadowCaster() const { return m_ClosestPointLightShadowCaster != nullptr; }
		Framebuffer* GetPointLightShadowCasterFramebuffer() { return m_PointLightShadowFramebuffer; }
		Cubemap* GetPointLightShadowCasterCubemap() { return m_PointLightShadowCubemap; }
		glm::vec3 GetPointLightShadowCasterLightPosition();
		glm::vec2 GetPointLightShadowCasterNearFarPlane();
		float GetPointLightShadowCasterBias();
		int GetPointLightShadowCasterIndex();
	private:
		void FindClosestDirectionalLightShadowCaster();
		void FindClosestSpotLightShadowCaster();
		void FindClosestPointLightShadowCaster();
		void BindLights(Shader *shader, bool bindOnlyStatic);
		void ReallocateTarget(Framebuffer **framebuffer, glm::uvec2 newResolution);
		void ReallocateCubemap(Cubemap** cubemap, glm::uvec2 newResolution);
	private:
		Scene *m_Scene;

		// Directional Light Shadows
		LightComponent *m_ClosestDirectionalLightShadowCaster;
		TransformComponent *m_ClosestDirectionalLightShadowCasterTransform;
		int m_ClosestDirectionalLightIndex = 0;
		Framebuffer *m_DirectionalLightShadowFramebuffer;

		// Spot Light Shadows
		LightComponent *m_ClosestSpotLightShadowCaster;
		TransformComponent *m_ClosestSpotLightShadowCasterTransform;
		int m_ClosestSpotLightIndex = 0;
		Framebuffer *m_SpotLightShadowFramebuffer;

		// Point Light Shadows
		LightComponent* m_ClosestPointLightShadowCaster;
		TransformComponent* m_ClosestPointLightShadowCasterTranform;
		int m_ClosestPointLightIndex = 0;
		Cubemap *m_PointLightShadowCubemap;
		Framebuffer* m_PointLightShadowFramebuffer;
	};
}
