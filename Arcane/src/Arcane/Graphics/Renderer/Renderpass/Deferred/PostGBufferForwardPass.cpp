#include "arcpch.h"
#include "PostGBufferForwardPass.h"

#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Graphics/Skybox.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Scene/Scene3D.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>

namespace Arcane
{
	PostGBufferForward::PostGBufferForward(Scene3D *scene) : RenderPass(scene)
	{
		m_ModelShader = ShaderLoader::LoadShader("forward/PBR_Model.glsl");
	}

	PostGBufferForward::~PostGBufferForward() {}

	LightingPassOutput PostGBufferForward::executeLightingPass(ShadowmapPassOutput &shadowmapData, LightingPassOutput &lightingPassData, ICamera *camera, bool renderOnlyStatic, bool useIBL) {
		glViewport(0, 0, lightingPassData.outputFramebuffer->GetWidth(), lightingPassData.outputFramebuffer->GetHeight());
		lightingPassData.outputFramebuffer->Bind();
		m_GLCache->SetMultisample(false);
		m_GLCache->SetDepthTest(true);

		// Setup
		ModelRenderer *modelRenderer = m_ActiveScene->GetModelRenderer();
		DynamicLightManager *lightManager = m_ActiveScene->GetDynamicLightManager();
		Skybox *skybox = m_ActiveScene->GetSkybox();
		ProbeManager *probeManager = m_ActiveScene->GetProbeManager();

		// Render skybox
		skybox->Draw(camera);

		// View setup + lighting setup
		auto lightBindFunction = &DynamicLightManager::BindLightingUniforms;
		if (renderOnlyStatic)
			lightBindFunction = &DynamicLightManager::BindStaticLightingUniforms;

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
		bindShadowmap(m_ModelShader, shadowmapData);

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
			m_ActiveScene->AddTransparentStaticModelsToRenderer();
		}
		else {
			m_ActiveScene->AddTransparentModelsToRenderer();
		}

		// Render transparent objects
		modelRenderer->SetupTransparentRenderState();
		modelRenderer->FlushTransparent(m_ModelShader, MaterialRequired);

		// Render pass output
		LightingPassOutput passOutput;
		passOutput.outputFramebuffer = lightingPassData.outputFramebuffer;
		return passOutput;
	}

	void PostGBufferForward::bindShadowmap(Shader *shader, ShadowmapPassOutput &shadowmapData) {
		shadowmapData.shadowmapFramebuffer->GetDepthStencilTexture()->Bind();
		shader->SetUniform("shadowmap", 0);
		shader->SetUniform("lightSpaceViewProjectionMatrix", shadowmapData.directionalLightViewProjMatrix);
	}
}
