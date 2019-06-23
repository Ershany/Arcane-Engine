#include "pch.h"
#include "DeferredLightingPass.h"

#include <utils/loaders/ShaderLoader.h>

namespace arcane {

	DeferredLightingPass::DeferredLightingPass(Scene3D *scene) : RenderPass(scene, RenderPassType::LightingPassType), m_AllocatedFramebuffer(true)
	{
		m_LightingShader = ShaderLoader::loadShader("src/shaders/deferred/pbr_lighting_pass.vert", "src/shaders/deferred/pbr_lighting_pass.frag");

		m_Framebuffer = new Framebuffer(Window::getWidth(), Window::getHeight());
		m_Framebuffer->addTexture2DColorAttachment(false).addDepthStencilRBO(false).createFramebuffer();
	}

	DeferredLightingPass::DeferredLightingPass(Scene3D *scene, Framebuffer *customFramebuffer) : RenderPass(scene, RenderPassType::LightingPassType), m_AllocatedFramebuffer(false), m_Framebuffer(customFramebuffer)
	{
		m_LightingShader = ShaderLoader::loadShader("src/shaders/deferred/pbr_lighting_pass.vert", "src/shaders/deferred/pbr_lighting_pass.frag");
	}

	DeferredLightingPass::~DeferredLightingPass() {
		if (m_AllocatedFramebuffer) {
			delete m_Framebuffer;
		}
	}

	LightingPassOutput DeferredLightingPass::executeRenderPass(ShadowmapPassOutput &shadowmapData, GeometryPassOutput &geometryData, ICamera *camera, bool useIBL) {
		glViewport(0, 0, m_Framebuffer->getWidth(), m_Framebuffer->getHeight());
		m_Framebuffer->bind();
		m_Framebuffer->clear();
		m_GLCache->setMultisample(false);

		// Setup
		DynamicLightManager *lightManager = m_ActiveScene->getDynamicLightManager();
		ProbeManager *probeManager = m_ActiveScene->getProbeManager();

		m_GLCache->switchShader(m_LightingShader);
		lightManager->bindLightingUniforms(m_LightingShader);
		m_LightingShader->setUniform3f("viewPos", camera->getPosition());
		m_LightingShader->setUniformMat4("viewInverse", glm::inverse(camera->getViewMatrix()));
		m_LightingShader->setUniformMat4("projectionInverse", glm::inverse(camera->getProjectionMatrix()));

		// Bind GBuffer data
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, geometryData.outputGBuffer->getAlbedo());
		m_LightingShader->setUniform1i("albedoTexture", 4);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, geometryData.outputGBuffer->getNormal());
		m_LightingShader->setUniform1i("normalTexture", 5);

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, geometryData.outputGBuffer->getMaterialInfo());
		m_LightingShader->setUniform1i("materialInfoTexture", 6);

		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, geometryData.outputGBuffer->getDepthTexture());
		m_LightingShader->setUniform1i("depthTexture", 7);

		m_LightingShader->setUniform1f("nearPlane", NEAR_PLANE);
		m_LightingShader->setUniform1f("farPlane", FAR_PLANE);

		// Shadowmap code
		bindShadowmap(m_LightingShader, shadowmapData);

		// IBL code
		if (useIBL) {
			m_LightingShader->setUniform1i("computeIBL", 1);
			probeManager->bindProbe(glm::vec3(0.0f, 0.0f, 0.0f), m_LightingShader);
		}
		else {
			m_LightingShader->setUniform1i("computeIBL", 0);
		}
    
		// Render a NDC quad
		ModelRenderer *modelRenderer = m_ActiveScene->getModelRenderer();
		modelRenderer->NDC_Plane.Draw();

		// Render pass output
		LightingPassOutput passOutput;
		passOutput.outputFramebuffer = m_Framebuffer;
		return passOutput;
	}

	void DeferredLightingPass::bindShadowmap(Shader *shader, ShadowmapPassOutput &shadowmapData) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowmapData.shadowmapFramebuffer->getDepthTexture());
		shader->setUniform1i("shadowmap", 0);
		shader->setUniformMat4("lightSpaceViewProjectionMatrix", shadowmapData.directionalLightViewProjMatrix);
	}

}
