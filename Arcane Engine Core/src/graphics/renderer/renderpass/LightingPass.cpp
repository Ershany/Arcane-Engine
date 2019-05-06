#include "pch.h"
#include "LightingPass.h"

#include <utils/loaders/ShaderLoader.h>

namespace arcane {

	LightingPass::LightingPass(Scene3D *scene, bool useIBL) : RenderPass(scene, RenderPassType::LightingPassType), m_UseIBL(useIBL)
	{
		m_ModelShader = ShaderLoader::loadShader("src/shaders/pbr_model.vert", "src/shaders/pbr_model.frag");
		m_TerrainShader = ShaderLoader::loadShader("src/shaders/terrain.vert", "src/shaders/terrain.frag");

		m_Framebuffer = new Framebuffer(Window::getWidth(), Window::getHeight());
		bool shouldMultisample = MSAA_SAMPLE_AMOUNT > 1.0 ? true : false;
		m_Framebuffer->addTexture2DColorAttachment(shouldMultisample).addDepthStencilRBO(shouldMultisample).createFramebuffer();
	}

	LightingPass::LightingPass(Scene3D *scene, Framebuffer *customFramebuffer, bool useIBL) : RenderPass(scene, RenderPassType::LightingPassType), m_Framebuffer(customFramebuffer), m_UseIBL(useIBL)
	{
		m_ModelShader = ShaderLoader::loadShader("src/shaders/pbr_model.vert", "src/shaders/pbr_model.frag");
		m_TerrainShader = ShaderLoader::loadShader("src/shaders/terrain.vert", "src/shaders/terrain.frag");
	}

	LightingPass::~LightingPass() {}

	LightingPassOutput LightingPass::executeRenderPass(ShadowmapPassOutput &shadowmapData, ICamera *camera) {
		glViewport(0, 0, m_Framebuffer->getWidth(), m_Framebuffer->getHeight());
		m_Framebuffer->bind();
		m_Framebuffer->clear();

		// Setup
		ModelRenderer *modelRenderer = m_ActiveScene->getModelRenderer();
		Terrain *terrain = m_ActiveScene->getTerrain();
		DynamicLightManager *lightManager = m_ActiveScene->getDynamicLightManager();
		Skybox *skybox = m_ActiveScene->getSkybox();
		ProbeManager *probeManager = m_ActiveScene->getProbeManager();

		// View setup + lighting setup
		m_GLCache->switchShader(m_ModelShader);
		lightManager->setupLightingUniforms(m_ModelShader);
		m_ModelShader->setUniform3f("viewPos", camera->getPosition());
		m_ModelShader->setUniformMat4("view", camera->getViewMatrix());
		m_ModelShader->setUniformMat4("projection", camera->getProjectionMatrix());

		// Shadowmap code
		bindShadowmap(m_ModelShader, shadowmapData);

		// IBL code
		if (m_UseIBL) {
			m_ModelShader->setUniform1i("computeIBL", 1);
			probeManager->bindProbe(glm::vec3(0.0f, 0.0f, 0.0f), m_ModelShader);
		}
		else {
			m_ModelShader->setUniform1i("computeIBL", 0);
		}

		// Render the scene
		m_ActiveScene->addModelsToRenderer();
		modelRenderer->flushOpaque(m_ModelShader, m_RenderPassType);

		m_GLCache->switchShader(m_TerrainShader);
		lightManager->setupLightingUniforms(m_TerrainShader);
		m_TerrainShader->setUniform3f("viewPos", camera->getPosition());
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), terrain->getPosition());
		glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
		m_TerrainShader->setUniformMat3("normalMatrix", normalMatrix);
		m_TerrainShader->setUniformMat4("model", modelMatrix);
		m_TerrainShader->setUniformMat4("view", camera->getViewMatrix());
		m_TerrainShader->setUniformMat4("projection", camera->getProjectionMatrix());
		bindShadowmap(m_TerrainShader, shadowmapData);

		terrain->Draw(m_TerrainShader, m_RenderPassType);

		skybox->Draw(camera);

		m_GLCache->switchShader(m_ModelShader);
		modelRenderer->flushTransparent(m_ModelShader, m_RenderPassType);

		// Render pass output
		LightingPassOutput passOutput;
		passOutput.outputFramebuffer = m_Framebuffer;
		return passOutput;
	}

	void LightingPass::bindShadowmap(Shader *shader, ShadowmapPassOutput &shadowmapData) {
		shader->setUniform1i("shadowmap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowmapData.shadowmapFramebuffer->getDepthTexture());
		shader->setUniformMat4("lightSpaceViewProjectionMatrix", shadowmapData.directionalLightViewProjMatrix);
	}

}
