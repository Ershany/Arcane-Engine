#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

namespace Arcane
{
	class Shader;
	class Texture;

	class Material {
	public:
		Material() = default;

		// Assumes the shader is already bound
		void BindMaterialInformation(Shader *shader) const;

		void SetAlbedoMap(Texture *texture);
		inline void SetNormalMap(Texture *texture) { m_NormalMap = texture; }
		inline void SetMetallicMap(Texture *texture) { m_MetallicMap = texture; }
		inline void SetRoughnessMap(Texture *texture) { m_RoughnessMap = texture; }
		inline void SetAmbientOcclusionMap(Texture *texture) { m_AmbientOcclusionMap = texture; }
		inline void SetDisplacementMap(Texture *texture) { m_DisplacementMap = texture; }
		void SetEmissionMap(Texture *texture);

		inline void SetAlbedoColour(glm::vec4 &value) { m_AlbedoColour = value; }
		inline void SetMetallicValue(float value) { m_MetallicValue = value; }
		inline void SetRoughnessValue(float value) { m_RoughnessValue = value; }
		inline void SetDisplacmentStrength(float strength) { m_ParallaxStrength = strength; }
		inline void SetDisplacementMinSteps(int steps) { m_ParallaxMinSteps = steps; }
		inline void SetDisplacementMaxSteps(int steps) { m_ParallaxMaxSteps = steps; }
		inline void SetEmissionIntensity(float intensity) { m_EmissionIntensity = intensity; }
		inline void SetEmissionColour(glm::vec3 colour) { m_EmissionColour = colour; }

		inline Texture* GetAlbedoMap() { return m_AlbedoMap; }
		inline Texture* GetNormalMap() { return m_NormalMap; }
		inline Texture* GetMetallicMap() { return m_MetallicMap; }
		inline Texture* GetRoughnessMap() { return m_RoughnessMap; }
		inline Texture* GetAmbientOcclusionMap() { return m_AmbientOcclusionMap; }
		inline Texture* GetDisplacementMap() { return m_DisplacementMap; }
		inline Texture* GetEmissionMap() { return m_EmissionMap; }

		inline glm::vec4& GetAlbedoColourRef() { return m_AlbedoColour; }
		inline float& GetMetallicValueRef() { return m_MetallicValue; }
		inline float& GetRoughnessValueRef() { return m_RoughnessValue; }
		inline float& GetDisplacementStrengthRef() { return m_ParallaxStrength; }
		inline int& GetDisplacementMinStepsRef() { return m_ParallaxMinSteps; }
		inline int& GetDisplacementMaxStepsRef() { return m_ParallaxMaxSteps; }
		inline float& GetEmissionIntensityRef() { return m_EmissionIntensity; }
		inline glm::vec3& GetEmissionColourRef() { return m_EmissionColour; }
	private:
		// Textures will be given precedence if provided over raw values
		Texture *m_AlbedoMap = nullptr, *m_NormalMap = nullptr, *m_MetallicMap = nullptr, *m_RoughnessMap = nullptr, *m_AmbientOcclusionMap = nullptr, *m_DisplacementMap = nullptr, *m_EmissionMap = nullptr;
		glm::vec4 m_AlbedoColour = glm::vec4(0.894f, 0.023f, 0.992f, 1.0f);
		float m_MetallicValue = 0.0f, m_RoughnessValue = 0.0f;

		// Parallax values
		float m_ParallaxStrength = 0.07f;
		int m_ParallaxMinSteps = PARALLAX_MIN_STEPS, m_ParallaxMaxSteps = PARALLAX_MAX_STEPS; // Will need to increase when parallax strength increases

		// Emission values
		float m_EmissionIntensity = 1.0f;
		glm::vec3 m_EmissionColour = glm::vec3(0.0f, 0.0f, 0.0f);
	};
}
#endif
