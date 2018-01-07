#include "Renderer.h"

namespace arcane { namespace graphics {

		Renderer::Renderer(Camera *camera) : m_Camera(camera)
		{
		}

		void Renderer::submitOpaque(Renderable3D *renderable) {
			m_OpaqueRenderQueue.push_back(renderable);
		}

		void Renderer::submitTransparent(Renderable3D *renderable) {
			m_TransparentRenderQueue.push_back(renderable);
		}

		void Renderer::flushOpaque(Shader &shader, Shader &outlineShader) {
			glEnable(GL_CULL_FACE);

			// Render opaque objects
			while (!m_OpaqueRenderQueue.empty()) {
				glEnable(GL_DEPTH_TEST);
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);

				Renderable3D *current = m_OpaqueRenderQueue.front();
				setupModelMatrix(current, shader);
				current->draw(shader);

				if (current->getShouldOutline()) {
					glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

					outlineShader.enable();
					setupModelMatrix(current, outlineShader, 1.05f);

					current->draw(outlineShader);
					outlineShader.disable();

					glStencilMask(0xFF);

					shader.enable();

					glClear(GL_STENCIL_BUFFER_BIT);
				}

				m_OpaqueRenderQueue.pop_front();
			}
		}

		void Renderer::flushTransparent(Shader &shader, Shader &outlineShader) {
			glDisable(GL_CULL_FACE);

			// Sort then render transparent objects (from back to front, does not account for rotations or scaling)
			std::sort(m_TransparentRenderQueue.begin(), m_TransparentRenderQueue.end(),
				[this](Renderable3D *a, Renderable3D *b) -> bool
			{
				return glm::length2(m_Camera->getPosition() - a->getPosition()) > glm::length2(m_Camera->getPosition() - b->getPosition());
			});
			while (!m_TransparentRenderQueue.empty()) {
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				Renderable3D *current = m_TransparentRenderQueue.front();
				setupModelMatrix(current, shader);
				current->draw(shader);

				if (current->getShouldOutline()) {
					glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

					outlineShader.enable();
					setupModelMatrix(current, outlineShader, 1.05f);

					current->draw(outlineShader);
					outlineShader.disable();

					glEnable(GL_DEPTH_TEST);
					glStencilMask(0xFF);

					shader.enable();

					glClear(GL_STENCIL_BUFFER_BIT);
				}

				glDisable(GL_BLEND);

				m_TransparentRenderQueue.pop_front();
			}
		}

		// TODO: Currently only support two levels in a hierarchical scene graph
		void Renderer::setupModelMatrix(Renderable3D *renderable, Shader &shader, float scaleFactor) {
			glm::mat4 model(1);
			glm::mat4 translate = glm::translate(glm::mat4(1.0f), renderable->getPosition());
			glm::mat4 rotate = glm::toMat4(renderable->getOrientation());
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), renderable->getScale() * scaleFactor);

			if (!renderable->getParent()) {
				model = translate * rotate * scale;
			}
			else {
				// Only apply scale locally
				model = glm::translate(glm::mat4(1.0f), renderable->getParent()->getPosition()) * glm::toMat4(renderable->getParent()->getOrientation()) * translate * rotate * scale;
			}

			shader.setUniformMat4("model", model);
		}

} }