#include "Material.h"

namespace arcane { namespace graphics {

	Material::Material(Texture *diffuseMap, Texture *specularMap, Texture *normalMap, Texture *emissionMap, float shininess)
		: m_DiffuseMap(diffuseMap), m_SpecularMap(specularMap), m_NormalMap(normalMap), m_EmissionMap(emissionMap), m_Shininess(shininess) {}


	void Material::BindMaterialInformation(Shader &shader) const {
		int currentTextureUnit = 0;

		shader.setUniform1i("material.texture_diffuse", currentTextureUnit);
		if (m_DiffuseMap) {
			m_DiffuseMap->Bind(currentTextureUnit++);
		}
		else {
			utils::TextureLoader::getDefaultDiffuse()->Bind(currentTextureUnit++);
		}

		shader.setUniform1i("material.texture_specular", currentTextureUnit);
		if (m_SpecularMap) {
			m_SpecularMap->Bind(currentTextureUnit++);
		}
		else {
			utils::TextureLoader::getDefaultSpecular()->Bind(currentTextureUnit++);
		}

		shader.setUniform1i("material.texture_normal", currentTextureUnit);
		if (m_NormalMap) {
			m_NormalMap->Bind(currentTextureUnit++);
		}
		else {
			utils::TextureLoader::getDefaultNormal()->Bind(currentTextureUnit++);
		}

		shader.setUniform1i("material.texture_emission", currentTextureUnit);
		if (m_EmissionMap) {
			m_EmissionMap->Bind(currentTextureUnit++);
		}
		else {
			utils::TextureLoader::getDefaultEmission()->Bind(currentTextureUnit++);
		}

		shader.setUniform1f("material.shininess", m_Shininess);
	}

} }