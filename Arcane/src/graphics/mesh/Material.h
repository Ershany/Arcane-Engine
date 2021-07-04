#pragma once

#include <graphics/Shader.h>
#include <graphics/texture/Texture.h>
#include <utils/loaders/TextureLoader.h>

namespace Arcane
{
	class Material {
	public:
		Material(Texture *albedoMap = nullptr, Texture *normalMap = nullptr, Texture *metallicMap = nullptr, Texture *roughnessMap = nullptr, 
				 Texture *ambientOcclusionMap = nullptr, Texture *displacementMap = nullptr);

		// Assumes the shader is already bound
		void BindMaterialInformation(Shader *shader) const;

		inline void SetAlbedoMap(Texture *texture) { m_AlbedoMap = texture; }
		inline void SetNormalMap(Texture *texture) { m_NormalMap = texture; }
		inline void SetMetallicMap(Texture *texture) { m_MetallicMap = texture; }
		inline void SetRoughnessMap(Texture *texture) { m_RoughnessMap = texture; }
		inline void SetAmbientOcclusionMap(Texture *texture) { m_AmbientOcclusionMap = texture; }
		inline void SetDisplacementMap(Texture *texture) { m_DisplacementMap = texture; }

		inline void SetDisplacmentStrength(float strength) { m_ParallaxStrength = strength; }
	private:
		Texture *m_AlbedoMap, *m_NormalMap, *m_MetallicMap, *m_RoughnessMap, *m_AmbientOcclusionMap, *m_DisplacementMap;
		float m_ParallaxStrength;
		int m_ParallaxMinSteps, m_ParallelMaxSteps; // Will need to increase when parallax strength increases
	};
}
