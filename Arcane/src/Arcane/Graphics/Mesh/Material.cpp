#include "arcpch.h"
#include "Material.h"

#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/texture/Texture.h>
#include <Arcane/Util/Loaders/AssetManager.h>

namespace Arcane
{
	void Material::SetAlbedoMap(Texture *texture)
	{
		if (texture == nullptr)
			return;

#ifdef ARC_DEV_BUILD
		if (!texture->GetTextureSettings().IsSRGB)
		{
			ARC_LOG_ERROR("Albedo texture isn't set to sRGB space - it won't be properly linearized");
		}
#endif
		m_AlbedoMap = texture;
		m_AlbedoColour = glm::vec4(1.0, 1.0, 1.0, 1.0);
	}

	void Material::SetEmissionMap(Texture *texture)
	{
		if (texture == nullptr)
		{
			return;
		}

#ifdef ARC_DEV_BUILD
		if (!texture->GetTextureSettings().IsSRGB)
		{
			ARC_LOG_ERROR("Emission texture isn't set to sRGB space - it won't be properly linearized");
		}
#endif

		m_EmissionMap = texture;
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

		if (m_DisplacementMap && m_DisplacementMap->IsGenerated())
		{
			shader->SetUniform("hasDisplacement", true);
			shader->SetUniform("minMaxDisplacementSteps", glm::vec2(m_ParallaxMinSteps, m_ParallaxMaxSteps));
			shader->SetUniform("parallaxStrength", m_ParallaxStrength);
			shader->SetUniform("material.texture_displacement", currentTextureUnit);
			m_DisplacementMap->Bind(currentTextureUnit++);
		}
		else
		{
			shader->SetUniform("hasDisplacement", false);
		}

		if (m_EmissionMap && m_EmissionMap->IsGenerated())
		{
			shader->SetUniform("hasEmission", true);
			shader->SetUniform("material.emissionIntensity", m_EmissionIntensity);
			shader->SetUniform("material.hasEmissionTexture", true);
			shader->SetUniform("material.texture_emission", currentTextureUnit);
			m_EmissionMap->Bind(currentTextureUnit++);
		}
		else if (m_EmissionColour.r != 0.0f || m_EmissionColour.g != 0.0f || m_EmissionColour.b != 0.0f)
		{
			shader->SetUniform("hasEmission", true);
			shader->SetUniform("material.emissionColour", m_EmissionColour);
			shader->SetUniform("material.emissionIntensity", m_EmissionIntensity);
			shader->SetUniform("material.hasEmissionTexture", false);
		}
		else
		{
			shader->SetUniform("hasEmission", false);
			shader->SetUniform("material.hasEmissionTexture", false);
			shader->SetUniform("material.emissionIntensity", 0.0f);
		}
	}
}
