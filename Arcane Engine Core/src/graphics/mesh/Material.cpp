#include "Material.h"
#include "../Window.h"

namespace arcane { namespace graphics {

	Material::Material(Texture *diffuseMap, Texture *specularMap, Texture *normalMap, Texture *emissionMap, float shininess)
		: m_DiffuseMap(diffuseMap), m_SpecularMap(specularMap), m_NormalMap(normalMap), m_EmissionMap(emissionMap), m_Shininess(shininess) {}


	void Material::BindMaterialInformation(Shader &shader) const {
		// Texture unit 0 is reserved for the shadowmap
		int currentTextureUnit = 1;

		shader.setUniform1i("material.texture_diffuse", currentTextureUnit);
		if (m_DiffuseMap) {
			m_DiffuseMap->bind(currentTextureUnit++);
		}
		else {
			utils::TextureLoader::getDefaultDiffuse()->bind(currentTextureUnit++);
		}

		shader.setUniform1i("material.texture_specular", currentTextureUnit);
		if (m_SpecularMap) {
			m_SpecularMap->bind(currentTextureUnit++);
		}
		else {
			utils::TextureLoader::getDefaultSpecular()->bind(currentTextureUnit++);
		}

		shader.setUniform1i("material.texture_normal", currentTextureUnit);
		if (m_NormalMap) {
			m_NormalMap->bind(currentTextureUnit++);
		}
		else {
			utils::TextureLoader::getDefaultNormal()->bind(currentTextureUnit++);
		}

		shader.setUniform1i("material.texture_emission", currentTextureUnit);
		if (m_EmissionMap) {
			m_EmissionMap->bind(currentTextureUnit++);
		}
		else {
			utils::TextureLoader::getDefaultEmission()->bind(currentTextureUnit++);
		}

		shader.setUniform1f("material.shininess", m_Shininess);
	}

} }