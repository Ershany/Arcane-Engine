#include "arcpch.h"
#include "ShadowmapPass.h"

#include <Arcane/Scene/Scene.h>
#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>

namespace Arcane
{
	ShadowmapPass::ShadowmapPass(Scene *scene) : RenderPass(scene)
	{
		m_ShadowmapShader = ShaderLoader::LoadShader("Shadowmap_Generation.glsl");
	}

	ShadowmapPass::ShadowmapPass(Scene *scene, Framebuffer *customDirectionalLightShadowFramebuffer, Framebuffer *customSpotLightShadowFramebuffer)
		: RenderPass(scene), m_CustomDirectionalLightShadowFramebuffer(customDirectionalLightShadowFramebuffer), m_CustomSpotLightShadowFramebuffer(customSpotLightShadowFramebuffer)
	{
		m_ShadowmapShader = ShaderLoader::LoadShader("Shadowmap_Generation.glsl");
	}

	ShadowmapPass::~ShadowmapPass()
	{

	}

	ShadowmapPassOutput ShadowmapPass::generateShadowmaps(ICamera *camera, bool renderOnlyStatic)
	{
		// Render pass output
		ShadowmapPassOutput passOutput;

		LightManager *lightManager = m_ActiveScene->GetLightManager();
		Framebuffer *shadowFramebuffer;

		// Directional Light Shadow Setup
		if (m_CustomDirectionalLightShadowFramebuffer)
		{
			shadowFramebuffer = m_CustomDirectionalLightShadowFramebuffer;
		}
		else
		{
			shadowFramebuffer = lightManager->GetDirectionalLightShadowCasterFramebuffer();
		}
		glViewport(0, 0, shadowFramebuffer->GetWidth(), shadowFramebuffer->GetHeight());
		shadowFramebuffer->Bind();
		shadowFramebuffer->ClearAll();

		// Directional Light Shadows
		if (lightManager->HasDirectionalLightShadowCaster())
		{
			// Setup
			Terrain *terrain = m_ActiveScene->GetTerrain();
			glm::vec2 nearFarPlane = lightManager->GetDirectionalLightShadowCasterNearFarPlane();

			// View + Projection setup
			m_GLCache->SetShader(m_ShadowmapShader);
			glm::vec3 dirLightShadowmapLookAtPos = camera->GetPosition();
			glm::vec3 dirLightShadowmapEyePos = dirLightShadowmapLookAtPos - (lightManager->GetDirectionalLightShadowCasterLightDir() * 50.0f);
			glm::mat4 directionalLightProjection = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, nearFarPlane.x, nearFarPlane.y);
			glm::mat4 directionalLightView = glm::lookAt(dirLightShadowmapEyePos, dirLightShadowmapLookAtPos, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 directionalLightViewProjMatrix = directionalLightProjection * directionalLightView;
			m_ShadowmapShader->SetUniform("lightSpaceViewProjectionMatrix", directionalLightViewProjMatrix);

			// Setup model renderer
			if (renderOnlyStatic)
			{
				m_ActiveScene->AddModelsToRenderer(ModelFilterType::StaticModels);
			}
			else
			{
				m_ActiveScene->AddModelsToRenderer(ModelFilterType::AllModels);
			}

			// Render models
			m_GLCache->SetDepthTest(true);
			m_GLCache->SetBlend(false);
			m_GLCache->SetFaceCull(false);
			Renderer::Flush(camera, m_ShadowmapShader, RenderPassType::NoMaterialRequired);

			// Render terrain
			terrain->Draw(m_ShadowmapShader, RenderPassType::NoMaterialRequired);

			// Update output
			passOutput.directionalLightViewProjMatrix = directionalLightViewProjMatrix;
			passOutput.directionalShadowmapBias = lightManager->GetDirectionalLightShadowCasterBias();
			passOutput.directionalShadowmapFramebuffer = shadowFramebuffer;
		}

		// Spot Light Shadow Setup
		if (m_CustomSpotLightShadowFramebuffer)
		{
			shadowFramebuffer = m_CustomSpotLightShadowFramebuffer;
		}
		else
		{
			shadowFramebuffer = lightManager->GetSpotLightShadowCasterFramebuffer();
		}
		glViewport(0, 0, shadowFramebuffer->GetWidth(), shadowFramebuffer->GetHeight());
		shadowFramebuffer->Bind();
		shadowFramebuffer->ClearAll();

		// Spot Light Shadows
		if (lightManager->HasSpotlightShadowCaster())
		{
			// Setup
			Terrain *terrain = m_ActiveScene->GetTerrain();
			glm::vec2 nearFarPlane = lightManager->GetSpotLightShadowCasterNearFarPlane();

			// View + Projection setup
			m_GLCache->SetShader(m_ShadowmapShader);
			float outerAngleRadians = lightManager->GetSpotLightShadowCasterOuterCutOffAngle();
			float radius = lightManager->GetSpotLightShadowCasterAttenuationRange() * glm::tan(outerAngleRadians); // Need to get spotlight's radius given it's range and angle so we can use it for the projection bounds
			float padding = 1.0f;
			glm::mat4 spotLightProjection = glm::ortho(-(radius + padding), radius + padding, -(radius + padding), radius + padding, nearFarPlane.x, nearFarPlane.y);
			glm::vec3 spotLightPos = lightManager->GetSpotLightShadowCasterLightPosition();
			glm::mat4 spotLightView = glm::lookAt(spotLightPos, spotLightPos + lightManager->GetSpotLightShadowCasterLightDir(), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 spotLightViewProjMatrix = spotLightProjection * spotLightView;
			m_ShadowmapShader->SetUniform("lightSpaceViewProjectionMatrix", spotLightViewProjMatrix);

			// Setup model renderer
			if (renderOnlyStatic)
			{
				m_ActiveScene->AddModelsToRenderer(ModelFilterType::StaticModels);
			}
			else
			{
				m_ActiveScene->AddModelsToRenderer(ModelFilterType::AllModels);
			}

			// Render models
			m_GLCache->SetDepthTest(true);
			m_GLCache->SetBlend(false);
			m_GLCache->SetFaceCull(false);
			Renderer::Flush(camera, m_ShadowmapShader, RenderPassType::NoMaterialRequired);

			// Render terrain
			terrain->Draw(m_ShadowmapShader, RenderPassType::NoMaterialRequired);

			// Update output
			passOutput.spotLightViewProjMatrix = spotLightViewProjMatrix;
			passOutput.spotLightShadowmapBias = lightManager->GetSpotLightShadowCasterBias();
			passOutput.spotLightShadowmapFramebuffer = shadowFramebuffer;
		}

		return passOutput;
	}
}
