#include "arcpch.h"
#include "DeferredLightingPass.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Graphics/Renderer/Renderpass/Deferred/DeferredGeometryPass.h>
#include <Arcane/Scene/Scene.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>

namespace Arcane
{
	DeferredLightingPass::DeferredLightingPass(Scene *scene) : RenderPass(scene), m_AllocatedFramebuffer(true)
	{
		m_LightingShader = ShaderLoader::LoadShader("deferred/PBR_LightingPass.glsl");

		m_Framebuffer = new Framebuffer(Window::GetRenderResolutionWidth(), Window::GetRenderResolutionHeight(), false);
		m_Framebuffer->AddColorTexture(FloatingPoint16).AddDepthStencilTexture(NormalizedDepthStencil).CreateFramebuffer();
	}

	DeferredLightingPass::DeferredLightingPass(Scene *scene, Framebuffer *customFramebuffer) : RenderPass(scene), m_AllocatedFramebuffer(false), m_Framebuffer(customFramebuffer)
	{
		m_LightingShader = ShaderLoader::LoadShader("deferred/PBR_LightingPass.glsl");
	}

	DeferredLightingPass::~DeferredLightingPass() {
		if (m_AllocatedFramebuffer) {
			delete m_Framebuffer;
		}
	}

	LightingPassOutput DeferredLightingPass::executeLightingPass(ShadowmapPassOutput &shadowmapData, GeometryPassOutput &geometryData, PreLightingPassOutput &preLightingOutput, ICamera *camera, bool useIBL) {
		// Framebuffer setup
		glViewport(0, 0, m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight());
		glViewport(0, 0, m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight());
		m_Framebuffer->Bind();
		m_Framebuffer->Clear();
		m_GLCache->SetDepthTest(false);
		m_GLCache->SetMultisample(false);

		// Move the depth + stencil of the GBuffer to our framebuffer
		// NOTE: Framebuffers have to have identical depth + stencil formats for this to work
		glBindFramebuffer(GL_READ_FRAMEBUFFER, geometryData.outputGBuffer->GetFramebuffer());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffer->GetFramebuffer());
		glBlitFramebuffer(0, 0, geometryData.outputGBuffer->GetWidth(), geometryData.outputGBuffer->GetHeight(), 0, 0, m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight(), GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);

		// Setup initial stencil state
		m_GLCache->SetStencilTest(true);
		m_GLCache->SetStencilWriteMask(0x00); // Do not update stencil values

		DynamicLightManager *lightManager = m_ActiveScene->GetDynamicLightManager();
		ProbeManager *probeManager = m_ActiveScene->GetProbeManager();

		m_GLCache->SetShader(m_LightingShader);
		lightManager->BindLightingUniforms(m_LightingShader);
		m_LightingShader->SetUniform("viewPos", camera->GetPosition());
		m_LightingShader->SetUniform("viewInverse", glm::inverse(camera->GetViewMatrix()));
		m_LightingShader->SetUniform("projectionInverse", glm::inverse(camera->GetProjectionMatrix()));

		// Bind GBuffer data
		geometryData.outputGBuffer->GetAlbedo()->Bind(4);
		m_LightingShader->SetUniform("albedoTexture", 4);

		geometryData.outputGBuffer->GetNormal()->Bind(5);
		m_LightingShader->SetUniform("normalTexture", 5);

		geometryData.outputGBuffer->GetMaterialInfo()->Bind(6);
		m_LightingShader->SetUniform("materialInfoTexture", 6);

		preLightingOutput.ssaoTexture->Bind(7);
		m_LightingShader->SetUniform("ssaoTexture", 7);

		geometryData.outputGBuffer->GetDepthStencilTexture()->Bind(8);
		m_LightingShader->SetUniform("depthTexture", 8);

		m_LightingShader->SetUniform("nearPlane", NEAR_PLANE);
		m_LightingShader->SetUniform("farPlane", FAR_PLANE);

		// Shadowmap code
		bindShadowmap(m_LightingShader, shadowmapData);

		// Finally perform the lighting using the GBuffer
		ModelRenderer *modelRenderer = m_ActiveScene->GetModelRenderer();

		// IBL Binding
		probeManager->BindProbes(glm::vec3(0.0f, 0.0f, 0.0f), m_LightingShader);

		// Perform lighting on the terrain (turn IBL off)
		m_LightingShader->SetUniform("computeIBL", 0);
		m_GLCache->SetStencilFunc(GL_EQUAL, DeferredStencilValue::TerrainStencilValue, 0xFF);
		modelRenderer->NDC_Plane.Draw();

		// Perform lighting on the models in the scene (turn IBL on)
		if (useIBL) {
			m_LightingShader->SetUniform("computeIBL", 1);
		}
		else {
			m_LightingShader->SetUniform("computeIBL", 0);
		}
		m_GLCache->SetStencilFunc(GL_EQUAL, DeferredStencilValue::ModelStencilValue, 0xFF);
		modelRenderer->NDC_Plane.Draw();


		// Reset state
		m_GLCache->SetDepthTest(true);
		m_GLCache->SetStencilTest(false);

		// Render pass output
		LightingPassOutput passOutput;
		passOutput.outputFramebuffer = m_Framebuffer;
		return passOutput;
	}

	void DeferredLightingPass::bindShadowmap(Shader *shader, ShadowmapPassOutput &shadowmapData) {
		shadowmapData.shadowmapFramebuffer->GetDepthStencilTexture()->Bind();
		shader->SetUniform("shadowmap", 0);
		shader->SetUniform("lightSpaceViewProjectionMatrix", shadowmapData.directionalLightViewProjMatrix);
	}
}
