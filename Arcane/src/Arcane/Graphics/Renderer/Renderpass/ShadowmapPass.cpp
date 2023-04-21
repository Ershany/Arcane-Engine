#include "arcpch.h"
#include "ShadowmapPass.h"

#include <Arcane/Scene/Scene.h>
#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Texture/Cubemap.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>

namespace Arcane
{
	ShadowmapPass::ShadowmapPass(Scene *scene) : RenderPass(scene), m_EmptyFramebuffer(1, 1, false)
	{
		Init();
	}

	ShadowmapPass::ShadowmapPass(Scene *scene, Framebuffer *customDirectionalLightShadowFramebuffer, Framebuffer *customSpotLightShadowFramebuffer, Cubemap* customPointLightShadowCubemap)
		: RenderPass(scene), m_EmptyFramebuffer(1, 1, false),
		m_CustomDirectionalLightShadowFramebuffer(customDirectionalLightShadowFramebuffer), m_CustomSpotLightShadowFramebuffer(customSpotLightShadowFramebuffer), m_CustomPointLightShadowCubemap(customPointLightShadowCubemap)
	{
		Init();
	}

	ShadowmapPass::~ShadowmapPass()
	{

	}

	void ShadowmapPass::Init()
	{
		m_ShadowmapShader = ShaderLoader::LoadShader("Shadowmap_Generation.glsl");
		m_ShadowmapLinearShader = ShaderLoader::LoadShader("Shadowmap_Generation_Linear.glsl");
		m_EmptyFramebuffer.AddDepthStencilTexture(NormalizedDepthOnly).CreateFramebuffer();
	}

	ShadowmapPassOutput ShadowmapPass::generateShadowmaps(ICamera *camera, bool renderOnlyStatic)
	{
		// Render pass output
		ShadowmapPassOutput passOutput;

		LightManager *lightManager = m_ActiveScene->GetLightManager();
		Framebuffer *shadowFramebuffer;

		m_GLCache->SetShader(m_ShadowmapShader);

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
		shadowFramebuffer->ClearDepth();

		// Directional Light Shadows
		if (lightManager->HasDirectionalLightShadowCaster())
		{
			// Setup
			Terrain *terrain = m_ActiveScene->GetTerrain();
			glm::vec2 nearFarPlane = lightManager->GetDirectionalLightShadowCasterNearFarPlane();

			// View + Projection setup
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
			Renderer::Flush(camera, m_ShadowmapShader, RenderPassType::NoMaterialRequired);  // TODO: This should not use the camera's position for sorting we are rendering shadow maps for lights

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
		shadowFramebuffer->ClearDepth();

		// Spot Light Shadows
		if (lightManager->HasSpotLightShadowCaster())
		{
			// Setup
			Terrain *terrain = m_ActiveScene->GetTerrain();
			glm::vec2 nearFarPlane = lightManager->GetSpotLightShadowCasterNearFarPlane();

			// View + Projection setup
			float outerAngleRadians = lightManager->GetSpotLightShadowCasterOuterCutOffAngle();
			float radius = lightManager->GetSpotLightShadowCasterAttenuationRange() * glm::tan(outerAngleRadians); // Need to get spotlight's radius given it's range and angle so we can use it for the projection bounds
			glm::mat4 spotLightProjection = glm::ortho(-radius, radius, -radius, radius, nearFarPlane.x, nearFarPlane.y);
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
			Renderer::Flush(camera, m_ShadowmapShader, RenderPassType::NoMaterialRequired); // TODO: This should not use the camera's position for sorting we are rendering shadow maps for lights

			// Render terrain
			terrain->Draw(m_ShadowmapShader, RenderPassType::NoMaterialRequired);

			// Update output
			passOutput.spotLightViewProjMatrix = spotLightViewProjMatrix;
			passOutput.spotLightShadowmapBias = lightManager->GetSpotLightShadowCasterBias();
			passOutput.spotLightShadowmapFramebuffer = shadowFramebuffer;
		}

		// Point Light Shadow Setup
		Cubemap *pointLightShadowCubemap = nullptr;
		if (m_CustomPointLightShadowCubemap)
		{
			pointLightShadowCubemap = m_CustomPointLightShadowCubemap;
		}
		else
		{
			pointLightShadowCubemap = lightManager->GetPointLightShadowCasterCubemap();
		}
		m_EmptyFramebuffer.Bind();

		// Point Light Shadows
		passOutput.hasPointLightShadows = false;
		if (lightManager->HasPointlightShadowCaster())
		{
			// Setup
			Terrain* terrain = m_ActiveScene->GetTerrain();
			glm::vec2 nearFarPlane = lightManager->GetPointLightShadowCasterNearFarPlane();

			// Camera Setup
			m_CubemapCamera.SetCenterPosition(lightManager->GetPointLightShadowCasterLightPosition());
			glm::mat4 pointLightProjection = m_CubemapCamera.GetProjectionMatrix(nearFarPlane.x, nearFarPlane.y);

			// Shader setup (point light uses custom shader)
			m_GLCache->SetShader(m_ShadowmapLinearShader);
			m_ShadowmapLinearShader->SetUniform("lightPos", m_CubemapCamera.GetPosition());
			m_ShadowmapLinearShader->SetUniform("lightFarPlane", nearFarPlane.y);

			// Render the scene to the probe's cubemap
			glViewport(0, 0, pointLightShadowCubemap->GetFaceWidth(), pointLightShadowCubemap->GetFaceHeight());
			for (int i = 0; i < 6; i++)
			{
				// Setup the camera's view
				m_CubemapCamera.SwitchCameraToFace(i);
				glm::mat4 pointLightView = m_CubemapCamera.GetViewMatrix();
				glm::mat4 pointLightViewProjMatrix = pointLightProjection * pointLightView;
				m_ShadowmapLinearShader->SetUniform("lightSpaceViewProjectionMatrix", pointLightViewProjMatrix);

				m_EmptyFramebuffer.SetDepthAttachment(DepthStencilAttachmentFormat::NormalizedDepthOnly, pointLightShadowCubemap->GetCubemapID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
				m_EmptyFramebuffer.ClearDepth();

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
				Renderer::Flush(camera, m_ShadowmapLinearShader, RenderPassType::NoMaterialRequired);  // TODO: This should not use the camera's position for sorting we are rendering shadow maps for lights

				// Render terrain
				terrain->Draw(m_ShadowmapLinearShader, RenderPassType::NoMaterialRequired);
			}
			// Reset state
			m_EmptyFramebuffer.SetDepthAttachment(DepthStencilAttachmentFormat::NormalizedDepthOnly, 0, GL_TEXTURE_CUBE_MAP_POSITIVE_X);

			// Update output
			passOutput.hasPointLightShadows = true;
			passOutput.pointLightShadowmapBias = lightManager->GetPointLightShadowCasterBias();
			passOutput.pointLightFarPlane = nearFarPlane.y;
		}
		passOutput.pointLightShadowCubemap = pointLightShadowCubemap; // Has to be bound even if it isn't used, thanks to OpenGL Driver

		return passOutput;
	}
}
