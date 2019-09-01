#pragma once

#include <graphics/Shader.h>
#include <graphics/texture/Texture.h>
#include <utils/loaders/TextureLoader.h>

namespace arcane {

	class Material {
	public:
		Material(Texture *albedoMap = nullptr, Texture *normalMap = nullptr, Texture *metallicMap = nullptr, Texture *roughnessMap = nullptr, 
				 Texture *ambientOcclusionMap = nullptr, Texture *displacementMap = nullptr);

		// Assumes the shader is already bound
		void BindMaterialInformation(Shader *shader) const;

		inline void setAlbedoMap(Texture *texture) { m_AlbedoMap = texture; }
		inline void setNormalMap(Texture *texture) { m_NormalMap = texture; }
		inline void setMetallicMap(Texture *texture) { m_MetallicMap = texture; }
		inline void setRoughnessMap(Texture *texture) { m_RoughnessMap = texture; }
		inline void setAmbientOcclusionMap(Texture *texture) { m_AmbientOcclusionMap = texture; }
		inline void setDisplacementMap(Texture *texture) { m_DisplacementMap = texture; }

		inline void setDisplacmentStrength(float strength) { m_ParallaxStrength = strength; }
	private:
		Texture *m_AlbedoMap, *m_NormalMap, *m_MetallicMap, *m_RoughnessMap, *m_AmbientOcclusionMap, *m_DisplacementMap;
		float m_ParallaxStrength;
		int m_ParallaxMinSteps, m_ParallelMaxSteps; // Will need to increase when parallax strength increases
	};

}
