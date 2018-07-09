#include "Material.h"

namespace arcane { namespace graphics {

	Material::Material(Shader *shader, unsigned int diffuseMap, unsigned int specularMap, unsigned int normalMap, unsigned int emissionMap, float shininess)
		: m_Shader(shader), m_DiffuseMap(diffuseMap), m_SpecularMap(specularMap), m_NormalMap(normalMap), m_EmissionMap(emissionMap), m_Shininess(shininess) {}


	void Material::BindMaterialInformation() {
		int currentTextureUnit = 0;

		if (m_DiffuseMap > 0) {
			glActiveTexture(GL_TEXTURE0);
			m_Shader->setUniform1i("material.texture_diffuse", currentTextureUnit++);
			glBindTexture(GL_TEXTURE_2D, m_DiffuseMap);
		}
		if (m_SpecularMap > 0) {
			glActiveTexture(GL_TEXTURE0 + currentTextureUnit);
			m_Shader->setUniform1i("material.texture_specular", currentTextureUnit++);
			glBindTexture(GL_TEXTURE_2D, m_SpecularMap);
		}
		if (m_NormalMap > 0) {
			glActiveTexture(GL_TEXTURE0 + currentTextureUnit);
			m_Shader->setUniform1i("material.texture_normal", currentTextureUnit++);
			glBindTexture(GL_TEXTURE_2D, m_NormalMap);
		}
		if (m_EmissionMap > 0) {
			glActiveTexture(GL_TEXTURE0 + currentTextureUnit);
			m_Shader->setUniform1i("material.texture_emission", currentTextureUnit++);
			glBindTexture(GL_TEXTURE_2D, m_EmissionMap);
		}

		m_Shader->setUniform1f("material.shininess", m_Shininess);
	}

} }