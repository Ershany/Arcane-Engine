#include "pch.h"
#include "LightingPass.h"

namespace arcane
{

	LightingPass::LightingPass(Scene3D *scene) : RenderPass(scene, RenderPassType::LightingPassType),
		m_Framebuffer(Window::getWidth(), Window::getHeight()), m_ModelShader("src/shaders/pbr_model.vert", "src/shaders/pbr_model.frag"), m_TerrainShader("src/shaders/terrain.vert", "src/shaders/terrain.frag")
	{
		bool shouldMultisample = MSAA_SAMPLE_AMOUNT > 1.0 ? true : false;
		m_Framebuffer.addTexture2DColorAttachment(shouldMultisample).addDepthStencilRBO(shouldMultisample).createFramebuffer();
	}

	LightingPass::~LightingPass() {}

	LightingPassOutput LightingPass::executeRenderPass(ShadowmapPassOutput shadowmapData) {
		glViewport(0, 0, m_Framebuffer.getWidth(), m_Framebuffer.getHeight());
		m_Framebuffer.bind();
		m_Framebuffer.clear();

		// Setup
		ModelRenderer *modelRenderer = m_ActiveScene->getModelRenderer();
		FPSCamera *camera = m_ActiveScene->getCamera();
		Terrain *terrain = m_ActiveScene->getTerrain();
		DynamicLightManager *lightManager = m_ActiveScene->getDynamicLightManager();
		Skybox *skybox = m_ActiveScene->getSkybox();

		// View setup + lighting setup
		m_GLCache->switchShader(m_ModelShader);
		lightManager->setupLightingUniforms(m_ModelShader);
		m_ModelShader.setUniform3f("viewPos", camera->getPosition());
		m_ModelShader.setUniformMat4("view", camera->getViewMatrix());
		m_ModelShader.setUniformMat4("projection", camera->getProjectionMatrix());

		// Shadowmap code
		m_ModelShader.setUniform1i("shadowmap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowmapData.shadowmapTexture);
		m_ModelShader.setUniformMat4("lightSpaceViewProjectionMatrix", shadowmapData.directionalLightViewProjMatrix);

		// IBL code
		m_ModelShader.setUniform1i("irradianceMap", 1);
		skybox->getSkyboxCubemap()->bind(1);

		// Render the scene
		m_ActiveScene->addModelsToRenderer();
		modelRenderer->flushOpaque(m_ModelShader, m_RenderPassType);

		m_GLCache->switchShader(m_TerrainShader);
		m_TerrainShader.setUniform1i("shadowmap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowmapData.shadowmapTexture);
		lightManager->setupLightingUniforms(m_TerrainShader);
		m_TerrainShader.setUniform3f("viewPos", camera->getPosition());
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), terrain->getPosition());
		glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
		m_TerrainShader.setUniformMat3("normalMatrix", normalMatrix);
		m_TerrainShader.setUniformMat4("model", modelMatrix);
		m_TerrainShader.setUniformMat4("view", camera->getViewMatrix());
		m_TerrainShader.setUniformMat4("projection", camera->getProjectionMatrix());
		m_TerrainShader.setUniformMat4("lightSpaceViewProjectionMatrix", shadowmapData.directionalLightViewProjMatrix);
		terrain->Draw(m_TerrainShader, m_RenderPassType);

		skybox->Draw();

		m_GLCache->switchShader(m_ModelShader);
		modelRenderer->flushTransparent(m_ModelShader, m_RenderPassType);

		// Render pass output
		LightingPassOutput passOutput;
		passOutput.outputFramebuffer = &m_Framebuffer;
		return passOutput;
	}

}
