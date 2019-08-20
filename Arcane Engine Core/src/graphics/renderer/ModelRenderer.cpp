#include "pch.h"
#include "ModelRenderer.h"

namespace arcane {

	ModelRenderer::ModelRenderer(FPSCamera *camera) :
		m_Camera(camera), NDC_Plane(), NDC_Cube()
	{
		// Configure and cache OpenGL state
		m_GLCache = GLCache::getInstance();
		m_GLCache->setDepthTest(true);
		m_GLCache->setBlend(false);
		m_GLCache->setFaceCull(true);
	}

	void ModelRenderer::submitOpaque(RenderableModel *renderable) {
		m_OpaqueRenderQueue.push_back(renderable);
	}

	void ModelRenderer::submitTransparent(RenderableModel *renderable) {
		m_TransparentRenderQueue.push_back(renderable);
	}

	void ModelRenderer::setupOpaqueRenderState() {
		m_GLCache->setDepthTest(true);
		m_GLCache->setBlend(false);
		m_GLCache->setFaceCull(true);
		m_GLCache->setCullFace(GL_BACK);
	}
	
	void ModelRenderer::setupTransparentRenderState() {
		m_GLCache->setDepthTest(true);
		m_GLCache->setBlend(true);
		m_GLCache->setFaceCull(false);
	}

	void ModelRenderer::flushOpaque(Shader *shader, RenderPassType pass) {
		m_GLCache->switchShader(shader);

		// Render opaque objects
		while (!m_OpaqueRenderQueue.empty()) {
			RenderableModel *current = m_OpaqueRenderQueue.front();

			setupModelMatrix(current, shader, pass);
			current->draw(shader, pass);

			m_OpaqueRenderQueue.pop_front();
		}
	}

	void ModelRenderer::flushTransparent(Shader *shader, RenderPassType pass) {
		m_GLCache->switchShader(shader);

		// Sort then render transparent objects (from back to front, does not account for rotations or scaling)
		std::sort(m_TransparentRenderQueue.begin(), m_TransparentRenderQueue.end(),
			[this](RenderableModel *a, RenderableModel *b) -> bool
		{
			return glm::length2(m_Camera->getPosition() - a->getPosition()) > glm::length2(m_Camera->getPosition() - b->getPosition());
		});
		while (!m_TransparentRenderQueue.empty()) {
			RenderableModel *current = m_TransparentRenderQueue.front();

			m_GLCache->setBlend(true);
			m_GLCache->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			setupModelMatrix(current, shader, pass);
			current->draw(shader, pass);

			m_TransparentRenderQueue.pop_front();
		}
	}

	// TODO: Currently only supports two levels for hierarchical transformations
	// Make it work with any number of levels
	void ModelRenderer::setupModelMatrix(RenderableModel *renderable, Shader *shader, RenderPassType pass) {
		glm::mat4 model(1);
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), renderable->getPosition());
		glm::mat4 rotate = glm::toMat4(renderable->getOrientation());
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), renderable->getScale());

		if (renderable->getParent()) {
			// Only apply scale locally
			model = glm::translate(glm::mat4(1.0f), renderable->getParent()->getPosition()) * glm::toMat4(renderable->getParent()->getOrientation()) * translate * rotate * scale;
		}
		else {
			model = translate * rotate * scale;
		}

		shader->setUniform("model", model);

		if (pass == MaterialRequired) {
			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
			shader->setUniform("normalMatrix", normalMatrix);
		}
	}

}
