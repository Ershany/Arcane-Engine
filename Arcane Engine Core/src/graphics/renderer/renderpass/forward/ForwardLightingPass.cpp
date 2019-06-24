#include "pch.h"
#include "ForwardLightingPass.h"

#include <utils/loaders/ShaderLoader.h>

namespace arcane {

	ForwardLightingPass::ForwardLightingPass(Scene3D *scene, bool shouldMultisample) : RenderPass(scene, RenderPassType::LightingPassType), m_AllocatedFramebuffer(true)
	{
		m_ModelShader = ShaderLoader::loadShader("src/shaders/forward/pbr_model.vert", "src/shaders/forward/pbr_model.frag");
		m_TerrainShader = ShaderLoader::loadShader("src/shaders/forward/pbr_terrain.vert", "src/shaders/forward/pbr_terrain.frag");

		m_Framebuffer = new Framebuffer(Window::getWidth(), Window::getHeight());
		m_Framebuffer->addTexture2DColorAttachment(shouldMultisample).addDepthStencilRBO(shouldMultisample).createFramebuffer();
	}

	ForwardLightingPass::ForwardLightingPass(Scene3D *scene, Framebuffer *customFramebuffer) : RenderPass(scene, RenderPassType::LightingPassType), m_AllocatedFramebuffer(false), m_Framebuffer(customFramebuffer)
	{
		m_ModelShader = ShaderLoader::loadShader("src/shaders/forward/pbr_model.vert", "src/shaders/forward/pbr_model.frag");
		m_TerrainShader = ShaderLoader::loadShader("src/shaders/forward/pbr_terrain.vert", "src/shaders/forward/pbr_terrain.frag");
	}

	ForwardLightingPass::~ForwardLightingPass() {
		if (m_AllocatedFramebuffer) {
			delete m_Framebuffer;
		}
	}

	LightingPassOutput ForwardLightingPass::executePostLightingPass(ShadowmapPassOutput &shadowmapData, ICamera *camera, bool renderOnlyStatic, bool useIBL) {
		glViewport(0, 0, m_Framebuffer->getWidth(), m_Framebuffer->getHeight());
		m_Framebuffer->bind();
		m_Framebuffer->clear();
		if (m_Framebuffer->isMultisampledColourBuffer()) {
			m_GLCache->setMultisample(true);
		}
		else {
			m_GLCache->setMultisample(false);
		}

		// Setup
		ModelRenderer *modelRenderer = m_ActiveScene->getModelRenderer();
		Terrain *terrain = m_ActiveScene->getTerrain();
		DynamicLightManager *lightManager = m_ActiveScene->getDynamicLightManager();
		Skybox *skybox = m_ActiveScene->getSkybox();
		ProbeManager *probeManager = m_ActiveScene->getProbeManager();

		// View setup + lighting setup
		auto lightBindFunction = &DynamicLightManager::bindLightingUniforms;
		if (renderOnlyStatic)
			lightBindFunction = &DynamicLightManager::bindStaticLightingUniforms;

		m_GLCache->switchShader(m_ModelShader);
		(lightManager->*lightBindFunction) (m_ModelShader);
		m_ModelShader->setUniform3f("viewPos", camera->getPosition());
		m_ModelShader->setUniformMat4("view", camera->getViewMatrix());
		m_ModelShader->setUniformMat4("projection", camera->getProjectionMatrix());

		// Shadowmap code
		bindShadowmap(m_ModelShader, shadowmapData);

		// IBL code
		if (useIBL) {
			m_ModelShader->setUniform1i("computeIBL", 1);
			probeManager->bindProbe(glm::vec3(0.0f, 0.0f, 0.0f), m_ModelShader);
		}
		else {
			m_ModelShader->setUniform1i("computeIBL", 0);
		}

		// Setup model renderer
		if (renderOnlyStatic) {
			m_ActiveScene->addStaticModelsToRenderer();
		}
		else {
			m_ActiveScene->addModelsToRenderer();
		}

		// Render opaque objects
		modelRenderer->flushOpaque(m_ModelShader, m_RenderPassType);

		// Render terrain
		m_GLCache->switchShader(m_TerrainShader);
		(lightManager->*lightBindFunction) (m_TerrainShader);
		m_TerrainShader->setUniform3f("viewPos", camera->getPosition());
		m_TerrainShader->setUniformMat4("view", camera->getViewMatrix());
		m_TerrainShader->setUniformMat4("projection", camera->getProjectionMatrix());
		bindShadowmap(m_TerrainShader, shadowmapData);
		terrain->Draw(m_TerrainShader, m_RenderPassType);

		// Render skybox
		skybox->Draw(camera);

		// Render transparent objects
		m_GLCache->switchShader(m_ModelShader);
		modelRenderer->flushTransparent(m_ModelShader, m_RenderPassType);

		// Render pass output
		LightingPassOutput passOutput;
		passOutput.outputFramebuffer = m_Framebuffer;
		return passOutput;
	}

	void ForwardLightingPass::bindShadowmap(Shader *shader, ShadowmapPassOutput &shadowmapData) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowmapData.shadowmapFramebuffer->getDepthTexture());
		shader->setUniform1i("shadowmap", 0);
		shader->setUniformMat4("lightSpaceViewProjectionMatrix", shadowmapData.directionalLightViewProjMatrix);
	}

}
