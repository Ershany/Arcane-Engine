#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

namespace Arcane
{
	class Shader;
	class Texture;

	class Material {
	public:
		Material();

		// Assumes the shader is already bound
		void BindMaterialInformation(Shader *shader) const;

		inline void SetAlbedoMap(Texture *texture)
		{
			m_AlbedoMap = texture;
			m_AlbedoColour = glm::vec4(1.0, 1.0, 1.0, 1.0);
		}
		inline void SetNormalMap(Texture *texture) { m_NormalMap = texture; }
		inline void SetMetallicMap(Texture *texture) { m_MetallicMap = texture; }
		inline void SetRoughnessMap(Texture *texture) { m_RoughnessMap = texture; }
		inline void SetAmbientOcclusionMap(Texture *texture) { m_AmbientOcclusionMap = texture; }
		inline void SetDisplacementMap(Texture *texture) { m_DisplacementMap = texture; }
		inline void SetEmissionMap(Texture *texture) { m_EmissionMap = texture; }

		inline void SetAlbedoColour(glm::vec4 &value) { m_AlbedoColour = value; }
		inline void SetMetallicValue(float value) { m_MetallicValue = value; }
		inline void SetRoughnessValue(float value) { m_RoughnessValue = value; }
		inline void SetDisplacmentStrength(float strength) { m_ParallaxStrength = strength; }
		inline void SetDisplacementMinSteps(int steps) { m_ParallaxMinSteps = steps; }
		inline void SetDisplacementMaxSteps(int steps) { m_ParallaxMaxSteps = steps; }
		inline void SetEmissionIntensity(float intensity) { m_EmissionIntensity = intensity; }

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
	private:
		// Textures will be given precedence if provided over raw values
		Texture *m_AlbedoMap, *m_NormalMap, *m_MetallicMap, *m_RoughnessMap, *m_AmbientOcclusionMap, *m_DisplacementMap, *m_EmissionMap;
		glm::vec4 m_AlbedoColour;
		float m_MetallicValue, m_RoughnessValue;

		// Parallax values
		float m_ParallaxStrength;
		int m_ParallaxMinSteps, m_ParallaxMaxSteps; // Will need to increase when parallax strength increases

		// Emission values
		float m_EmissionIntensity = 5.0f;
	};
}
#endif
