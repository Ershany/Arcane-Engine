#include "arcpch.h"
#include "ForwardLightingPass.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Skybox.h>
#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Graphics/Texture/Cubemap.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Scene/Scene.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>
#include <Arcane/Platform/OpenGL/Framebuffer/Framebuffer.h>

namespace Arcane
{
	ForwardLightingPass::ForwardLightingPass(Scene *scene, bool shouldMultisample) : RenderPass(scene), m_AllocatedFramebuffer(true)
	{
		Init();

		m_Framebuffer = new Framebuffer(Window::GetRenderResolutionWidth(), Window::GetRenderResolutionHeight(), shouldMultisample);
		m_Framebuffer->AddColorTexture(FloatingPoint16).AddDepthStencilRBO(NormalizedDepthStencil).CreateFramebuffer();
	}

	ForwardLightingPass::ForwardLightingPass(Scene *scene, Framebuffer *customFramebuffer) : RenderPass(scene), m_AllocatedFramebuffer(false), m_Framebuffer(customFramebuffer)
	{
		Init();
	}

	ForwardLightingPass::~ForwardLightingPass()
	{
		if (m_AllocatedFramebuffer) {
			delete m_Framebuffer;
		}
	}

	void ForwardLightingPass::Init()
	{
		m_ModelShader = ShaderLoader::LoadShader("forward/PBR_Model.glsl");
		m_SkinnedModelShader = ShaderLoader::LoadShader("forward/PBR_Skinned_Model.glsl");
		m_TerrainShader = ShaderLoader::LoadShader("forward/PBR_Terrain.glsl");
	}

	LightingPassOutput ForwardLightingPass::ExecuteOpaqueLightingPass(ShadowmapPassOutput &inputShadowmapData, ICamera *camera, bool renderOnlyStatic, bool useIBL)
	{
		glViewport(0, 0, m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight());
		m_Framebuffer->Bind();
		m_Framebuffer->ClearAll();
		if (m_Framebuffer->IsMultisampled()) {
			m_GLCache->SetMultisample(true);
		}
		else {
			m_GLCache->SetMultisample(false);
		}

		// Setup
		Terrain *terrain = m_ActiveScene->GetTerrain();
		LightManager *lightManager = m_ActiveScene->GetLightManager();
		ProbeManager *probeManager = m_ActiveScene->GetProbeManager();

		// Lighting setup
		auto lightBindFunction = &LightManager::BindLightingUniforms;
		if (renderOnlyStatic)
			lightBindFunction = &LightManager::BindStaticLightingUniforms;

		// Render terrain
		m_GLCache->SetShader(m_TerrainShader);
		if (m_GLCache->GetUsesClipPlane())
		{
			m_TerrainShader->SetUniform("usesClipPlane", true);
			m_TerrainShader->SetUniform("clipPlane", m_GLCache->GetActiveClipPlane());
		}
		else
		{
			m_TerrainShader->SetUniform("usesClipPlane", false);
		}
		(lightManager->*lightBindFunction) (m_TerrainShader);
		m_TerrainShader->SetUniform("viewPos", camera->GetPosition());
		m_TerrainShader->SetUniform("view", camera->GetViewMatrix());
		m_TerrainShader->SetUniform("projection", camera->GetProjectionMatrix());
		BindShadowmap(m_TerrainShader, inputShadowmapData);
		terrain->Draw(m_TerrainShader, MaterialRequired);

		// Render opaque objects since we are in the opaque pass
		// Add meshes to the renderer
		if (renderOnlyStatic)
		{
			m_ActiveScene->AddModelsToRenderer(ModelFilterType::OpaqueStaticModels);
		}
		else
		{
			m_ActiveScene->AddModelsToRenderer(ModelFilterType::OpaqueModels);
		}

		// Bind data to skinned shader and render skinned models
		{
			m_GLCache->SetShader(m_SkinnedModelShader);
			if (m_GLCache->GetUsesClipPlane())
			{
				m_SkinnedModelShader->SetUniform("usesClipPlane", true);
				m_SkinnedModelShader->SetUniform("clipPlane", m_GLCache->GetActiveClipPlane());
			}
			else
			{
				m_SkinnedModelShader->SetUniform("usesClipPlane", false);
			}
			(lightManager->*lightBindFunction) (m_SkinnedModelShader);

			// Shadowmap code
			BindShadowmap(m_SkinnedModelShader, inputShadowmapData);

			// IBL Binding
			glm::vec3 cameraPosition = camera->GetPosition();
			probeManager->BindProbes(cameraPosition, m_SkinnedModelShader); // TODO: Should use camera component
			if (useIBL)
			{
				m_SkinnedModelShader->SetUniform("computeIBL", 1);
			}
			else
			{
				m_SkinnedModelShader->SetUniform("computeIBL", 0);
			}

			Renderer::FlushOpaqueSkinnedMeshes(camera, RenderPassType::MaterialRequired, m_SkinnedModelShader);
		}

		// Bind data to non-skinned shader and render non-skinned models
		{
			m_GLCache->SetShader(m_ModelShader);
			if (m_GLCache->GetUsesClipPlane())
			{
				m_ModelShader->SetUniform("usesClipPlane", true);
				m_ModelShader->SetUniform("clipPlane", m_GLCache->GetActiveClipPlane());
			}
			else
			{
				m_ModelShader->SetUniform("usesClipPlane", false);
			}
			(lightManager->*lightBindFunction) (m_ModelShader);

			// Shadowmap code
			BindShadowmap(m_ModelShader, inputShadowmapData);

			// IBL Binding
			glm::vec3 cameraPosition = camera->GetPosition();
			probeManager->BindProbes(cameraPosition, m_ModelShader); // TODO: Should use camera component
			if (useIBL)
			{
				m_ModelShader->SetUniform("computeIBL", 1);
			}
			else
			{
				m_ModelShader->SetUniform("computeIBL", 0);
			}

			Renderer::FlushOpaqueNonSkinnedMeshes(camera, RenderPassType::MaterialRequired, m_ModelShader);
		}

		// Render pass output
		LightingPassOutput passOutput;
		passOutput.outputFramebuffer = m_Framebuffer;
		return passOutput;
	}

	LightingPassOutput ForwardLightingPass::ExecuteTransparentLightingPass(ShadowmapPassOutput &inputShadowmapData, Framebuffer *inputFramebuffer, ICamera *camera, bool renderOnlyStatic, bool useIBL)
	{
		glViewport(0, 0, inputFramebuffer->GetWidth(), inputFramebuffer->GetHeight());
		inputFramebuffer->Bind();
		if (inputFramebuffer->IsMultisampled())
		{
			m_GLCache->SetMultisample(true);
		}
		else
		{
			m_GLCache->SetMultisample(false);
		}
		m_GLCache->SetDepthTest(true);

		// Setup
		LightManager *lightManager = m_ActiveScene->GetLightManager();
		Skybox *skybox = m_ActiveScene->GetSkybox();
		ProbeManager *probeManager = m_ActiveScene->GetProbeManager();

		// Render skybox
		skybox->Draw(camera);

		// Lighting setup
		auto lightBindFunction = &LightManager::BindLightingUniforms;
		if (renderOnlyStatic)
			lightBindFunction = &LightManager::BindStaticLightingUniforms;

		// Render transparent objects since we are in the transparent pass
		// Add meshes to the renderer
		if (renderOnlyStatic)
		{
			m_ActiveScene->AddModelsToRenderer(ModelFilterType::TransparentStaticModels);
		}
		else
		{
			m_ActiveScene->AddModelsToRenderer(ModelFilterType::TransparentModels);
		}

		// Bind data to skinned shader and render skinned models
		{
			m_GLCache->SetShader(m_SkinnedModelShader);
			if (m_GLCache->GetUsesClipPlane())
			{
				m_SkinnedModelShader->SetUniform("usesClipPlane", true);
				m_SkinnedModelShader->SetUniform("clipPlane", m_GLCache->GetActiveClipPlane());
			}
			else
			{
				m_SkinnedModelShader->SetUniform("usesClipPlane", false);
			}
			(lightManager->*lightBindFunction) (m_SkinnedModelShader);

			// Shadowmap code
			BindShadowmap(m_SkinnedModelShader, inputShadowmapData);

			// IBL Binding
			glm::vec3 cameraPosition = camera->GetPosition();
			probeManager->BindProbes(cameraPosition, m_SkinnedModelShader); // TODO: Should use camera component
			if (useIBL)
			{
				m_SkinnedModelShader->SetUniform("computeIBL", 1);
			}
			else
			{
				m_SkinnedModelShader->SetUniform("computeIBL", 0);
			}

			Renderer::FlushTransparentSkinnedMeshes(camera, RenderPassType::MaterialRequired, m_SkinnedModelShader);
		}

		// Bind data to non-skinned shader and render non-skinned models
		{
			m_GLCache->SetShader(m_ModelShader);
			if (m_GLCache->GetUsesClipPlane())
			{
				m_ModelShader->SetUniform("usesClipPlane", true);
				m_ModelShader->SetUniform("clipPlane", m_GLCache->GetActiveClipPlane());
			}
			else
			{
				m_ModelShader->SetUniform("usesClipPlane", false);
			}
			(lightManager->*lightBindFunction) (m_ModelShader);

			// Shadowmap code
			BindShadowmap(m_ModelShader, inputShadowmapData);

			// IBL Binding
			glm::vec3 cameraPosition = camera->GetPosition();
			probeManager->BindProbes(cameraPosition, m_ModelShader); // TODO: Should use camera component
			if (useIBL)
			{
				m_ModelShader->SetUniform("computeIBL", 1);
			}
			else
			{
				m_ModelShader->SetUniform("computeIBL", 0);
			}

			Renderer::FlushTransparentNonSkinnedMeshes(camera, RenderPassType::MaterialRequired, m_ModelShader);
		}

		// Render pass output
		LightingPassOutput passOutput;
		passOutput.outputFramebuffer = inputFramebuffer;
		return passOutput;
	}

	void ForwardLightingPass::BindShadowmap(Shader *shader, ShadowmapPassOutput &shadowmapData)
	{
		LightManager *lightManager = m_ActiveScene->GetLightManager();

		bool hasDirShadowMap = shadowmapData.directionalShadowmapFramebuffer != nullptr;
		bool hasSpotShadowMap = shadowmapData.spotLightShadowmapFramebuffer != nullptr;

		shader->SetUniform("dirLightShadowData.lightShadowIndex", hasDirShadowMap ? lightManager->GetDirectionalLightShadowCasterIndex() : -1);
		shader->SetUniform("spotLightShadowData.lightShadowIndex", hasSpotShadowMap ? lightManager->GetSpotLightShadowCasterIndex() : -1);
		shader->SetUniform("pointLightShadowData.lightShadowIndex", shadowmapData.hasPointLightShadows ? lightManager->GetPointLightShadowCasterIndex() : -1);

		if (hasDirShadowMap)
		{
			shadowmapData.directionalShadowmapFramebuffer->GetDepthStencilTexture()->Bind(0);
			shader->SetUniform("dirLightShadowmap", 0);
			shader->SetUniform("dirLightShadowData.lightSpaceViewProjectionMatrix", shadowmapData.directionalLightViewProjMatrix);
			shader->SetUniform("dirLightShadowData.shadowBias", shadowmapData.directionalShadowmapBias);
		}
		if (hasSpotShadowMap)
		{
			shadowmapData.spotLightShadowmapFramebuffer->GetDepthStencilTexture()->Bind(1);
			shader->SetUniform("spotLightShadowmap", 1);
			shader->SetUniform("spotLightShadowData.lightSpaceViewProjectionMatrix", shadowmapData.spotLightViewProjMatrix);
			shader->SetUniform("spotLightShadowData.shadowBias", shadowmapData.spotLightShadowmapBias);
		}
		if (shadowmapData.hasPointLightShadows)
		{
			shader->SetUniform("pointLightShadowData.shadowBias", shadowmapData.pointLightShadowmapBias);
			shader->SetUniform("pointLightShadowData.farPlane", shadowmapData.pointLightFarPlane);
		}
		shader->SetUniform("pointLightShadowCubemap", 2);
		shadowmapData.pointLightShadowCubemap->Bind(2); // Must be bound even if there is no point light shadows. Thanks OpenGL Driver!
	}
}
