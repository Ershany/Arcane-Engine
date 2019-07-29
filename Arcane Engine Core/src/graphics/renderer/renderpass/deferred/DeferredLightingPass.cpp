#include "pch.h"
#include "DeferredLightingPass.h"

#include <graphics/renderer/renderpass/deferred/DeferredGeometryPass.h>
#include <utils/loaders/ShaderLoader.h>

namespace arcane {

	DeferredLightingPass::DeferredLightingPass(Scene3D *scene) : RenderPass(scene), m_AllocatedFramebuffer(true)
	{
		m_LightingShader = ShaderLoader::loadShader("src/shaders/deferred/pbr_lighting_pass.vert", "src/shaders/deferred/pbr_lighting_pass.frag");

		m_Framebuffer = new Framebuffer(Window::getResolutionWidth(), Window::getResolutionHeight(), false);
		m_Framebuffer->addColorTexture(FloatingPoint16).addDepthStencilTexture(NormalizedDepthStencil).createFramebuffer();
	}

	DeferredLightingPass::DeferredLightingPass(Scene3D *scene, Framebuffer *customFramebuffer) : RenderPass(scene), m_AllocatedFramebuffer(false), m_Framebuffer(customFramebuffer)
	{
		m_LightingShader = ShaderLoader::loadShader("src/shaders/deferred/pbr_lighting_pass.vert", "src/shaders/deferred/pbr_lighting_pass.frag");
	}

	DeferredLightingPass::~DeferredLightingPass() {
		if (m_AllocatedFramebuffer) {
			delete m_Framebuffer;
		}
	}

	LightingPassOutput DeferredLightingPass::executeLightingPass(ShadowmapPassOutput &shadowmapData, GeometryPassOutput &geometryData, PreLightingPassOutput &preLightingOutput, ICamera *camera, bool useIBL) {
		// Framebuffer setup
		glViewport(0, 0, m_Framebuffer->getWidth(), m_Framebuffer->getHeight());
		m_Framebuffer->bind();
		m_Framebuffer->clear();
		m_GLCache->setDepthTest(false);
		m_GLCache->setMultisample(false);

		// Move the depth + stencil of the GBuffer to the our framebuffer
		// NOTE: Framebuffers have to have identical depth + stencil formats for this to work
		glBindFramebuffer(GL_READ_FRAMEBUFFER, geometryData.outputGBuffer->getFramebuffer());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffer->getFramebuffer());
		glBlitFramebuffer(0, 0, geometryData.outputGBuffer->getWidth(), geometryData.outputGBuffer->getHeight(), 0, 0, m_Framebuffer->getWidth(), m_Framebuffer->getHeight(), GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);

		// Setup initial stencil state
		m_GLCache->setStencilTest(true);
		m_GLCache->setStencilWriteMask(0x00); // Do not update stencil values

		DynamicLightManager *lightManager = m_ActiveScene->getDynamicLightManager();
		ProbeManager *probeManager = m_ActiveScene->getProbeManager();

		m_GLCache->switchShader(m_LightingShader);
		lightManager->bindLightingUniforms(m_LightingShader);
		m_LightingShader->setUniform("viewPos", camera->getPosition());
		m_LightingShader->setUniform("viewInverse", glm::inverse(camera->getViewMatrix()));
		m_LightingShader->setUniform("projectionInverse", glm::inverse(camera->getProjectionMatrix()));

		// Bind GBuffer data
		geometryData.outputGBuffer->getAlbedo()->bind(4);
		m_LightingShader->setUniform("albedoTexture", 4);

		geometryData.outputGBuffer->getNormal()->bind(5);
		m_LightingShader->setUniform("normalTexture", 5);

		geometryData.outputGBuffer->getMaterialInfo()->bind(6);
		m_LightingShader->setUniform("materialInfoTexture", 6);

		preLightingOutput.ssaoTexture->bind(7);
		m_LightingShader->setUniform("ssaoTexture", 7);

		geometryData.outputGBuffer->getDepthStencilTexture()->bind(8);
		m_LightingShader->setUniform("depthTexture", 8);

		m_LightingShader->setUniform("nearPlane", NEAR_PLANE);
		m_LightingShader->setUniform("farPlane", FAR_PLANE);

		// Shadowmap code
		bindShadowmap(m_LightingShader, shadowmapData);

		// Finally perform the lighting using the GBuffer
		ModelRenderer *modelRenderer = m_ActiveScene->getModelRenderer();

		// IBL Binding
		probeManager->bindProbes(glm::vec3(0.0f, 0.0f, 0.0f), m_LightingShader);

		// Perform lighting on the terrain (turn IBL off)
		m_LightingShader->setUniform("computeIBL", 0);
		glStencilFunc(GL_EQUAL, DeferredStencilValue::TerrainStencilValue, 0xFF);
		modelRenderer->NDC_Plane.Draw();

		// Perform lighting on the models in the scene (turn IBL on)
		if (useIBL) {
			m_LightingShader->setUniform("computeIBL", 1);
		}
		glStencilFunc(GL_EQUAL, DeferredStencilValue::ModelStencilValue, 0xFF);
		modelRenderer->NDC_Plane.Draw();


		// Reset state
		m_GLCache->setDepthTest(true);
		m_GLCache->setStencilTest(false);

		// Render pass output
		LightingPassOutput passOutput;
		passOutput.outputFramebuffer = m_Framebuffer;
		return passOutput;
	}

	void DeferredLightingPass::bindShadowmap(Shader *shader, ShadowmapPassOutput &shadowmapData) {
		shadowmapData.shadowmapFramebuffer->getDepthStencilTexture()->bind();
		shader->setUniform("shadowmap", 0);
		shader->setUniform("lightSpaceViewProjectionMatrix", shadowmapData.directionalLightViewProjMatrix);
	}

}
