#include "arcpch.h"
#include "Material.h"

#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/texture/Texture.h>
#include <Arcane/Util/Loaders/AssetManager.h>

namespace Arcane
{
	Material::Material()
		: m_AlbedoMap(nullptr), m_NormalMap(nullptr), m_MetallicMap(nullptr), m_RoughnessMap(nullptr), m_AmbientOcclusionMap(nullptr), m_DisplacementMap(nullptr), m_EmissionMap(nullptr),
			m_AlbedoColour(0.894f, 0.023f, 0.992f, 1.0f), m_MetallicValue(0.0f), m_RoughnessValue(0.0f),
			m_ParallaxStrength(0.07f), m_ParallaxMinSteps(PARALLAX_MIN_STEPS), m_ParallaxMaxSteps(PARALLAX_MAX_STEPS)
	{
	}


	void Material::BindMaterialInformation(Shader *shader) const
	{
		// Texture unit 0 is reserved for the directional shadowmap
		// Texture unit 1 is reserved for the spotlight shadowmap
		// Texture unit 2 is reserved for the pointlight shadowmap
		// Texture unit 3 is reserved for the irradianceMap used for indirect diffuse IBL
		// Texture unit 4 is reserved for the prefilterMap used for indirect specular IBL
		// Texture unit 5 is reserved for the brdfLUT used for indirect specular IBL
		int currentTextureUnit = 6;

		shader->SetUniform("material.albedoColour", m_AlbedoColour);
		if (m_AlbedoMap && m_AlbedoMap->IsGenerated())
		{
			shader->SetUniform("material.texture_albedo", currentTextureUnit);
			shader->SetUniform("material.hasAlbedoTexture", true);
			m_AlbedoMap->Bind(currentTextureUnit++);
		}
		else
		{
			shader->SetUniform("material.hasAlbedoTexture", false);
		}

		shader->SetUniform("material.texture_normal", currentTextureUnit);
		if (m_NormalMap && m_NormalMap->IsGenerated())
		{
			m_NormalMap->Bind(currentTextureUnit++);
		}
		else
		{
			AssetManager::GetInstance().GetDefaultNormalTexture()->Bind(currentTextureUnit++);
		}

		if (m_MetallicMap && m_MetallicMap->IsGenerated())
		{
			shader->SetUniform("material.texture_metallic", currentTextureUnit);
			shader->SetUniform("material.hasMetallicTexture", true);
			m_MetallicMap->Bind(currentTextureUnit++);
		}
		else
		{
			shader->SetUniform("material.hasMetallicTexture", false);
			shader->SetUniform("material.metallicValue", m_MetallicValue);
		}

		if (m_RoughnessMap && m_RoughnessMap->IsGenerated())
		{
			shader->SetUniform("material.texture_roughness", currentTextureUnit);
			shader->SetUniform("material.hasRoughnessTexture", true);
			m_RoughnessMap->Bind(currentTextureUnit++);
		}
		else
		{
			shader->SetUniform("material.hasRoughnessTexture", false);
			shader->SetUniform("material.roughnessValue", m_RoughnessValue);
		}

		shader->SetUniform("material.texture_ao", currentTextureUnit);
		if (m_AmbientOcclusionMap && m_AmbientOcclusionMap->IsGenerated())
		{
			m_AmbientOcclusionMap->Bind(currentTextureUnit++);
		}
		else
		{
			AssetManager::GetInstance().GetDefaultAOTexture()->Bind(currentTextureUnit++);
		}

		shader->SetUniform("material.texture_displacement", currentTextureUnit);
		if (m_DisplacementMap && m_DisplacementMap->IsGenerated())
		{
			shader->SetUniform("hasDisplacement", true);
			shader->SetUniform("minMaxDisplacementSteps", glm::vec2(m_ParallaxMinSteps, m_ParallaxMaxSteps));
			shader->SetUniform("parallaxStrength", m_ParallaxStrength);
			m_DisplacementMap->Bind(currentTextureUnit++);
		}
		else
		{
			shader->SetUniform("hasDisplacement", false);
		}

		shader->SetUniform("material.texture_emission", currentTextureUnit);
		if (m_EmissionMap && m_EmissionMap->IsGenerated())
		{
			shader->SetUniform("hasEmission", true);
			shader->SetUniform("material.emissionIntensity", m_EmissionIntensity);
			m_EmissionMap->Bind(currentTextureUnit++);
		}
		else
		{
			shader->SetUniform("hasEmission", false);
			shader->SetUniform("material.emissionIntensity", 0.0f);
		}
	}
}
