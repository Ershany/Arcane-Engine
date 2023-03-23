#include "arcpch.h"
#include "PostGBufferForwardPass.h"

#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Graphics/Skybox.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Scene/Scene.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>

namespace Arcane
{
	PostGBufferForward::PostGBufferForward(Scene *scene) : RenderPass(scene)
	{
		m_ModelShader = ShaderLoader::LoadShader("forward/PBR_Model.glsl");
	}

	PostGBufferForward::~PostGBufferForward() {}

	LightingPassOutput PostGBufferForward::ExecuteLightingPass(ShadowmapPassOutput &inputShadowmapData, Framebuffer *inputFramebuffer, ICamera *camera, bool renderOnlyStatic, bool useIBL)
	{
		glViewport(0, 0, inputFramebuffer->GetWidth(), inputFramebuffer->GetHeight());
		inputFramebuffer->Bind();
		m_GLCache->SetMultisample(false);
		m_GLCache->SetDepthTest(true);

		// Setup
		LightManager *lightManager = m_ActiveScene->GetLightManager();
		Skybox *skybox = m_ActiveScene->GetSkybox();
		ProbeManager *probeManager = m_ActiveScene->GetProbeManager();

		// Render skybox
		skybox->Draw(camera);

		// View setup + lighting setup
		auto lightBindFunction = &LightManager::BindLightingUniforms;
		if (renderOnlyStatic)
			lightBindFunction = &LightManager::BindStaticLightingUniforms;

		m_GLCache->SetShader(m_ModelShader);
		if (m_GLCache->GetUsesClipPlane()) {
			m_ModelShader->SetUniform("usesClipPlane", true);
			m_ModelShader->SetUniform("clipPlane", m_GLCache->GetActiveClipPlane());
		}
		else {
			m_ModelShader->SetUniform("usesClipPlane", false);
		}
		(lightManager->*lightBindFunction) (m_ModelShader);
		m_ModelShader->SetUniform("viewPos", camera->GetPosition());
		m_ModelShader->SetUniform("view", camera->GetViewMatrix());
		m_ModelShader->SetUniform("projection", camera->GetProjectionMatrix());

		// Shadowmap code
		BindShadowmap(m_ModelShader, inputShadowmapData);

		// IBL code
		if (useIBL) {
			m_ModelShader->SetUniform("computeIBL", 1);
			probeManager->BindProbes(glm::vec3(0.0f, 0.0f, 0.0f), m_ModelShader);
		}
		else {
			m_ModelShader->SetUniform("computeIBL", 0);
		}

		// Render only transparent materials since we already rendered opaque using deferred
		if (renderOnlyStatic) {
			m_ActiveScene->AddModelsToRenderer(ModelFilterType::TransparentStaticModels);
		}
		else {
			m_ActiveScene->AddModelsToRenderer(ModelFilterType::TransparentModels);
		}

		// Render transparent objects
		Renderer::Flush(camera, m_ModelShader, RenderPassType::MaterialRequired);

		// Render pass output
		LightingPassOutput passOutput;
		passOutput.outputFramebuffer = inputFramebuffer;
		return passOutput;
	}

	void PostGBufferForward::BindShadowmap(Shader *shader, ShadowmapPassOutput &shadowmapData)
	{
		bool hasShadowMap = shadowmapData.directionalShadowmapFramebuffer != nullptr;
		shader->SetUniform("hasDirectionalShadow", hasShadowMap);
		if (!hasShadowMap)
			return;

		shadowmapData.directionalShadowmapFramebuffer->GetDepthStencilTexture()->Bind();
		shader->SetUniform("shadowmap", 0);
		shader->SetUniform("lightSpaceViewProjectionMatrix", shadowmapData.directionalLightViewProjMatrix);
		shader->SetUniform("shadowBias", shadowmapData.directionalShadowmapBias);
	}
}
