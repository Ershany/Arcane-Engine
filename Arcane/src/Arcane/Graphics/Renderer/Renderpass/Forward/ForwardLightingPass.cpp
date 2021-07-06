#include "arcpch.h"
#include "ForwardLightingPass.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Skybox.h>
#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Scene/Scene3D.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>

namespace Arcane
{
	ForwardLightingPass::ForwardLightingPass(Scene3D *scene, bool shouldMultisample) : RenderPass(scene), m_AllocatedFramebuffer(true)
	{
		m_ModelShader = ShaderLoader::loadShader("src/shaders/forward/PBR_Model.glsl");
		m_TerrainShader = ShaderLoader::loadShader("src/shaders/forward/PBR_Terrain.glsl");

		m_Framebuffer = new Framebuffer(Window::GetRenderResolutionWidth(), Window::GetRenderResolutionHeight(), shouldMultisample);
		m_Framebuffer->AddColorTexture(FloatingPoint16).AddDepthStencilRBO(NormalizedDepthStencil).CreateFramebuffer();
	}

	ForwardLightingPass::ForwardLightingPass(Scene3D *scene, Framebuffer *customFramebuffer) : RenderPass(scene), m_AllocatedFramebuffer(false), m_Framebuffer(customFramebuffer)
	{
		m_ModelShader = ShaderLoader::loadShader("src/shaders/forward/PBR_Model.glsl");
		m_TerrainShader = ShaderLoader::loadShader("src/shaders/forward/PBR_Terrain.glsl");
	}

	ForwardLightingPass::~ForwardLightingPass() {
		if (m_AllocatedFramebuffer) {
			delete m_Framebuffer;
		}
	}

	LightingPassOutput ForwardLightingPass::executeLightingPass(ShadowmapPassOutput &shadowmapData, ICamera *camera, bool renderOnlyStatic, bool useIBL) {
		glViewport(0, 0, m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight());
		m_Framebuffer->Bind();
		m_Framebuffer->Clear();
		if (m_Framebuffer->IsMultisampled()) {
			m_GLCache->SetMultisample(true);
		}
		else {
			m_GLCache->SetMultisample(false);
		}

		// Setup
		ModelRenderer *modelRenderer = m_ActiveScene->GetModelRenderer();
		Terrain *terrain = m_ActiveScene->GetTerrain();
		DynamicLightManager *lightManager = m_ActiveScene->GetDynamicLightManager();
		Skybox *skybox = m_ActiveScene->GetSkybox();
		ProbeManager *probeManager = m_ActiveScene->GetProbeManager();

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

		// IBL Binding
		probeManager->BindProbes(glm::vec3(0.0f, 0.0f, 0.0f), m_ModelShader);

		// Setup model renderer
		if (renderOnlyStatic) {
			m_ActiveScene->AddStaticModelsToRenderer();
		}
		else {
			m_ActiveScene->AddModelsToRenderer();
		}

		// Render opaque objects
		if (useIBL) {
			m_ModelShader->SetUniform("computeIBL", 1);
		}
		else {
			m_ModelShader->SetUniform("computeIBL", 0);
		}
		modelRenderer->SetupOpaqueRenderState();
		modelRenderer->FlushOpaque(m_ModelShader, MaterialRequired);

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
		bindShadowmap(m_TerrainShader, shadowmapData);
		terrain->Draw(m_TerrainShader, MaterialRequired);

		// Render skybox
		skybox->Draw(camera);

		// Render transparent objects
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
		if (useIBL) {
			probeManager->BindProbes(glm::vec3(0.0f, 0.0f, 0.0f), m_ModelShader);
		}
		modelRenderer->SetupTransparentRenderState();
		modelRenderer->FlushTransparent(m_ModelShader, MaterialRequired);

		// Render pass output
		LightingPassOutput passOutput;
		passOutput.outputFramebuffer = m_Framebuffer;
		return passOutput;
	}

	void ForwardLightingPass::bindShadowmap(Shader *shader, ShadowmapPassOutput &shadowmapData) {
		shadowmapData.shadowmapFramebuffer->GetDepthStencilTexture()->Bind();
		shader->SetUniform("shadowmap", 0);
		shader->SetUniform("lightSpaceViewProjectionMatrix", shadowmapData.directionalLightViewProjMatrix);
	}
}
