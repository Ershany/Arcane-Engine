#include "arcpch.h"
#include "Material.h"

#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/texture/Texture.h>
#include <Arcane/Util/Loaders/TextureLoader.h>
#include <Arcane/Graphics/Window.h>
#include <Arcane/UI/DebugPane.h>

namespace Arcane
{
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

		shader->SetUniform("material.texture_albedo", currentTextureUnit);
		if (m_AlbedoMap) {
			m_AlbedoMap->Bind(currentTextureUnit++);
		}
		else {
			TextureLoader::GetDefaultAlbedo()->Bind(currentTextureUnit++);
		}

		shader->SetUniform("material.texture_normal", currentTextureUnit);
		if (m_NormalMap) {
			m_NormalMap->Bind(currentTextureUnit++);
		}
		else {
			TextureLoader::GetDefaultNormal()->Bind(currentTextureUnit++);
		}

		shader->SetUniform("material.texture_metallic", currentTextureUnit);
		if (m_MetallicMap) {
			m_MetallicMap->Bind(currentTextureUnit++);
		}
		else {
			TextureLoader::GetDefaultMetallic()->Bind(currentTextureUnit++);
		}

		shader->SetUniform("material.texture_roughness", currentTextureUnit);
		if (m_RoughnessMap) {
			m_RoughnessMap->Bind(currentTextureUnit++);
		}
		else {
			TextureLoader::GetDefaultRoughness()->Bind(currentTextureUnit++);
		}

		shader->SetUniform("material.texture_ao", currentTextureUnit);
		if (m_AmbientOcclusionMap) {
			m_AmbientOcclusionMap->Bind(currentTextureUnit++);
		}
		else {
			TextureLoader::GetDefaultAO()->Bind(currentTextureUnit++);
		}

		shader->SetUniform("material.texture_displacement", currentTextureUnit);
		if (m_DisplacementMap) {
			shader->SetUniform("hasDisplacement", true);
			shader->SetUniform("minMaxDisplacementSteps", glm::vec2(m_ParallaxMinSteps, m_ParallelMaxSteps));
			shader->SetUniform("parallaxStrength", m_ParallaxStrength);
			m_DisplacementMap->Bind(currentTextureUnit++);
		}
		else {
			shader->SetUniform("hasDisplacement", false);
		}
	}
}
