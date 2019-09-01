#include "pch.h"
#include "Material.h"

#include <graphics/Window.h>
#include <ui/DebugPane.h>

namespace arcane {

	Material::Material(Texture *albedoMap, Texture *normalMap, Texture *metallicMap, Texture *roughnessMap, Texture *ambientOcclusionMap, Texture *displacementMap)
		: m_AlbedoMap(albedoMap), m_NormalMap(normalMap), m_MetallicMap(metallicMap), m_RoughnessMap(roughnessMap), m_AmbientOcclusionMap(ambientOcclusionMap), m_DisplacementMap(displacementMap),
			m_ParallaxStrength(0.07f), m_ParallaxMinSteps(PARALLAX_MIN_STEPS), m_ParallelMaxSteps(PARALLAX_MAX_STEPS)
	{
	}


	void Material::BindMaterialInformation(Shader *shader) const {
		// Texture unit 0 is reserved for the shadowmap
		// Texture unit 1 is reserved for the irradianceMap used for indirect diffuse IBL
		// Texture unit 2 is reserved for the prefilterMap
		// Texture unit 3 is reserved for the brdfLUT
		int currentTextureUnit = 4;

		shader->setUniform("material.texture_albedo", currentTextureUnit);
		if (m_AlbedoMap) {
			m_AlbedoMap->bind(currentTextureUnit++);
		}
		else {
			TextureLoader::getDefaultAlbedo()->bind(currentTextureUnit++);
		}

		shader->setUniform("material.texture_normal", currentTextureUnit);
		if (m_NormalMap) {
			m_NormalMap->bind(currentTextureUnit++);
		}
		else {
			TextureLoader::getDefaultNormal()->bind(currentTextureUnit++);
		}

		shader->setUniform("material.texture_metallic", currentTextureUnit);
		if (m_MetallicMap) {
			m_MetallicMap->bind(currentTextureUnit++);
		}
		else {
			TextureLoader::getDefaultMetallic()->bind(currentTextureUnit++);
		}

		shader->setUniform("material.texture_roughness", currentTextureUnit);
		if (m_RoughnessMap) {
			m_RoughnessMap->bind(currentTextureUnit++);
		}
		else {
			TextureLoader::getDefaultRoughness()->bind(currentTextureUnit++);
		}

		shader->setUniform("material.texture_ao", currentTextureUnit);
		if (m_AmbientOcclusionMap) {
			m_AmbientOcclusionMap->bind(currentTextureUnit++);
		}
		else {
			TextureLoader::getDefaultAO()->bind(currentTextureUnit++);
		}

		shader->setUniform("material.texture_displacement", currentTextureUnit);
		if (m_DisplacementMap) {
			shader->setUniform("hasDisplacement", true);
			shader->setUniform("minMaxDisplacementSteps", glm::vec2(m_ParallaxMinSteps, m_ParallelMaxSteps));
			shader->setUniform("parallaxStrength", m_ParallaxStrength);
			m_DisplacementMap->bind(currentTextureUnit++);
		}
		else {
			shader->setUniform("hasDisplacement", false);
		}
	}

}
