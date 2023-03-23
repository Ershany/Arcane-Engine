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

	ShadowmapPass::ShadowmapPass(Scene *scene, Framebuffer *customFramebuffer) : RenderPass(scene), m_CustomShadowFramebuffer(customFramebuffer)
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
		if (m_CustomShadowFramebuffer)
		{
			shadowFramebuffer = m_CustomShadowFramebuffer;
		}
		else
		{
			shadowFramebuffer = lightManager->GetDirectionalShadowCasterFramebuffer();
		}

		glViewport(0, 0, shadowFramebuffer->GetWidth(), shadowFramebuffer->GetHeight());
		shadowFramebuffer->Bind();
		shadowFramebuffer->ClearAll();

		// Directional Light Shadows
		if (lightManager->HasDirectionalShadowCaster())
		{
			// Setup
			Terrain *terrain = m_ActiveScene->GetTerrain();
			glm::vec2 nearFarPlane = lightManager->GetDirectionalShadowCasterNearFarPlane();

			// View setup
			m_GLCache->SetShader(m_ShadowmapShader);
			glm::vec3 dirLightShadowmapLookAtPos = camera->GetPosition();
			glm::vec3 dirLightShadowmapEyePos = dirLightShadowmapLookAtPos - (lightManager->GetDirectionalShadowCasterLightDir() * 50.0f);
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
			passOutput.directionalShadowmapBias = lightManager->GetDirectionalShadowCasterBias();
			passOutput.directionalShadowmapFramebuffer = shadowFramebuffer;
		}

		return passOutput;
	}
}
