#include "Material.h"
#include "../Window.h"

namespace arcane { namespace graphics {

	Material::Material(Texture *albedoMap, Texture *normalMap, Texture *metallicMap, Texture *roughnessMap, Texture *ambientOcclusionMap, Texture *emissionMap)
		: m_AlbedoMap(albedoMap), m_NormalMap(normalMap), m_MetallicMap(metallicMap), m_RoughnessMap(roughnessMap), m_AmbientOcclusionMap(ambientOcclusionMap), m_EmissionMap(emissionMap) {}


	void Material::BindMaterialInformation(Shader &shader) const {
		// Texture unit 0 is reserved for the shadowmap
		int currentTextureUnit = 1;

		shader.setUniform1i("material.texture_albedo", currentTextureUnit);
		if (m_AlbedoMap) {
			m_AlbedoMap->bind(currentTextureUnit++);
		}
		else {
			utils::TextureLoader::getDefaultAlbedo()->bind(currentTextureUnit++);
		}

		shader.setUniform1i("material.texture_normal", currentTextureUnit);
		if (m_NormalMap) {
			m_NormalMap->bind(currentTextureUnit++);
		}
		else {
			utils::TextureLoader::getDefaultNormal()->bind(currentTextureUnit++);
		}

		shader.setUniform1i("material.texture_metallic", currentTextureUnit);
		if (m_MetallicMap) {
			m_MetallicMap->bind(currentTextureUnit++);
		}
		else {
			utils::TextureLoader::getDefaultMetallic()->bind(currentTextureUnit++);
		}

		shader.setUniform1i("material.texture_roughness", currentTextureUnit);
		if (m_RoughnessMap) {
			m_RoughnessMap->bind(currentTextureUnit++);
		}
		else {
			utils::TextureLoader::getDefaultRoughness()->bind(currentTextureUnit++);
		}

		shader.setUniform1i("material.texture_ao", currentTextureUnit);
		if (m_AmbientOcclusionMap) {
			m_AmbientOcclusionMap->bind(currentTextureUnit++);
		}
		else {
			utils::TextureLoader::getDefaultAO()->bind(currentTextureUnit++);
		}

		shader.setUniform1i("material.texture_emission", currentTextureUnit);
		if (m_EmissionMap) {
			m_EmissionMap->bind(currentTextureUnit++);
		}
		else {
			utils::TextureLoader::getDefaultEmission()->bind(currentTextureUnit++);
		}
	}

} }