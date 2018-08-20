#include "Renderer.h"

namespace arcane { namespace graphics {

		Renderer::Renderer(Camera *camera) : m_Camera(camera)
		{
			// Configure and cache OpenGL state
			m_GLCache = GLCache::getInstance();
			m_GLCache->setDepthTest(true);
			m_GLCache->setBlend(false);
			m_GLCache->setFaceCull(true);
		}

		void Renderer::submitOpaque(Renderable3D *renderable) {
			m_OpaqueRenderQueue.push_back(renderable);
		}

		void Renderer::submitTransparent(Renderable3D *renderable) {
			m_TransparentRenderQueue.push_back(renderable);
		}

		void Renderer::flushOpaque(Shader &shader, RenderPass pass) {
			m_GLCache->switchShader(shader.getShaderID());
			m_GLCache->setDepthTest(true);
			m_GLCache->setBlend(false);
			m_GLCache->setStencilTest(false);
			m_GLCache->setFaceCull(true);

			// Only draw the back face to depth buffers in order to avoid shadow acne
			if (pass == RenderPass::ShadowmapPass)
				m_GLCache->setCullFace(GL_FRONT);
			else
				m_GLCache->setCullFace(GL_BACK);

			// Render opaque objects
			while (!m_OpaqueRenderQueue.empty()) {
				Renderable3D *current = m_OpaqueRenderQueue.front();

				setupModelMatrix(current, shader, pass);
				current->draw(shader, pass);

				m_OpaqueRenderQueue.pop_front();
			}
		}

		void Renderer::flushTransparent(Shader &shader, RenderPass pass) {
			m_GLCache->switchShader(shader.getShaderID());
			m_GLCache->setDepthTest(true);
			m_GLCache->setBlend(true);
			m_GLCache->setStencilTest(false);

			// Only draw the back face to depth buffers in order to avoid shadow acne
			if (pass == RenderPass::ShadowmapPass)
				m_GLCache->setCullFace(GL_FRONT);
			else
				m_GLCache->setFaceCull(false);

			// Sort then render transparent objects (from back to front, does not account for rotations or scaling)
			std::sort(m_TransparentRenderQueue.begin(), m_TransparentRenderQueue.end(),
				[this](Renderable3D *a, Renderable3D *b) -> bool
			{
				return glm::length2(m_Camera->getPosition() - a->getPosition()) > glm::length2(m_Camera->getPosition() - b->getPosition());
			});
			while (!m_TransparentRenderQueue.empty()) {
				Renderable3D *current = m_TransparentRenderQueue.front();

				m_GLCache->setBlend(true);
				m_GLCache->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				setupModelMatrix(current, shader, pass);
				current->draw(shader, pass);

				m_TransparentRenderQueue.pop_front();
			}
		}

		// TODO: Currently only supports two levels for hierarchical transformations
		// Make it work with any number of levels
		void Renderer::setupModelMatrix(Renderable3D *renderable, Shader &shader, RenderPass pass) {
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

			shader.setUniformMat4("model", model);

			if (pass != RenderPass::ShadowmapPass) {
				glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
				shader.setUniformMat3("normalMatrix", normalMatrix);
			}
		}

} }