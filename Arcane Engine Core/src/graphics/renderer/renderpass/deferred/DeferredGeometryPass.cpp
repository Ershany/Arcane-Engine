#include "pch.h"
#include "DeferredGeometryPass.h"

#include <utils/loaders/ShaderLoader.h>

namespace arcane {

	DeferredGeometryPass::DeferredGeometryPass(Scene3D *scene) : RenderPass(scene, RenderPassType::GeometryPassType), m_AllocatedGBuffer(true) {
		m_ModelShader = ShaderLoader::loadShader("src/shaders/deferred/pbr_model_geometry.vert", "src/shaders/deferred/pbr_model_geometry.frag");
		m_TerrainShader = ShaderLoader::loadShader("src/shaders/deferred/pbr_terrain_geometry.vert", "src/shaders/deferred/pbr_terrain_geometry.frag");

		m_GBuffer = new GBuffer(Window::getWidth(), Window::getHeight());
	}

	DeferredGeometryPass::DeferredGeometryPass(Scene3D *scene, GBuffer *customGBuffer) : RenderPass(scene, RenderPassType::GeometryPassType), m_AllocatedGBuffer(false), m_GBuffer(customGBuffer) {
		m_ModelShader = ShaderLoader::loadShader("src/shaders/deferred/pbr_model_geometry.vert", "src/shaders/deferred/pbr_model_geometry.frag");
		m_TerrainShader = ShaderLoader::loadShader("src/shaders/deferred/pbr_terrain_geometry.vert", "src/shaders/deferred/pbr_terrain_geometry.frag");
	}

	DeferredGeometryPass::~DeferredGeometryPass() {
		if (m_AllocatedGBuffer) {
			delete m_GBuffer;
		}
	}

	GeometryPassOutput DeferredGeometryPass::executePostLightingPass(ICamera *camera, bool renderOnlyStatic) {
		glViewport(0, 0, m_GBuffer->getWidth(), m_GBuffer->getHeight());
		m_GBuffer->bind();
		m_GBuffer->clear();
		m_GLCache->setMultisample(false);

		// Setup
		ModelRenderer *modelRenderer = m_ActiveScene->getModelRenderer();
		Terrain *terrain = m_ActiveScene->getTerrain();

		m_GLCache->switchShader(m_ModelShader);
		m_ModelShader->setUniformMat4("view", camera->getViewMatrix());
		m_ModelShader->setUniformMat4("projection", camera->getProjectionMatrix());

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
		m_TerrainShader->setUniformMat4("view", camera->getViewMatrix());
		m_TerrainShader->setUniformMat4("projection", camera->getProjectionMatrix());
		terrain->Draw(m_TerrainShader, m_RenderPassType);

		// Render pass output
		GeometryPassOutput passOutput;
		passOutput.outputGBuffer = m_GBuffer;
		return passOutput;
	}

}
