#include "Renderer.h"

namespace arcane { namespace graphics {

	Renderer::Renderer(FPSCamera *camera) : m_Camera(camera)
	{ 
	}

	void Renderer::submitOpaque(Renderable3D *renderable) {
		m_OpaqueRenderQueue.push_back(renderable);
	}

	void Renderer::submitTransparent(Renderable3D *renderable) {
		m_TransparentRenderQueue.push_back(renderable);
	}

	void Renderer::flush(Shader &shader, Shader &outlineShader) {
		// Render opaque objects
		glEnable(GL_CULL_FACE);
		while (!m_OpaqueRenderQueue.empty()) {
			Renderable3D *current = m_OpaqueRenderQueue.front();

			// Drawing prepration
			glEnable(GL_DEPTH_TEST);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);

			// Draw the renderable 3d
			glm::mat4 model(1);
			model = glm::translate(model, current->getPosition());
			if ((current->getRotationAxis().x != 0 || current->getRotationAxis().y != 0 || current->getRotationAxis().z != 0) && current->getRadianRotation() != 0)
				model = glm::rotate(model, current->getRadianRotation(), current->getRotationAxis());
			model = glm::scale(model, current->getScale());
			shader.setUniformMat4("model", model);
			current->draw(shader);

			// Draw the outline
			if (current->getShouldOutline()) {
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

				outlineShader.enable();
				model = glm::mat4(1);
				model = glm::translate(model, current->getPosition());
				if ((current->getRotationAxis().x != 0 || current->getRotationAxis().y != 0 || current->getRotationAxis().z != 0) && current->getRadianRotation() != 0)
					model = glm::rotate(model, current->getRadianRotation(), current->getRotationAxis());
				model = glm::scale(model, current->getScale() + glm::vec3(0.025f, 0.025f, 0.025f));
				outlineShader.setUniformMat4("model", model);
				current->draw(outlineShader);
				outlineShader.disable();

				glEnable(GL_DEPTH_TEST);
				glStencilMask(0xFF);

				shader.enable();

				glClear(GL_STENCIL_BUFFER_BIT);
			}

			m_OpaqueRenderQueue.pop_front();
		}

		// Sort then render transparent objects (from back to front, does not account for rotations or scaling)
		glDisable(GL_CULL_FACE); // Don't backface cull transparent objects
		std::sort(m_TransparentRenderQueue.begin(), m_TransparentRenderQueue.end(), 
			[this](Renderable3D *a, Renderable3D *b) -> bool 
		{
			return glm::length2(m_Camera->getPosition() - a->getPosition()) > glm::length2(m_Camera->getPosition() - b->getPosition());
		});
		while (!m_TransparentRenderQueue.empty()) {
			Renderable3D *current = m_TransparentRenderQueue.front();

			// Drawing prepration
			glEnable(GL_DEPTH_TEST);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);

			// Enable blending (note: You will still need to sort from back to front when rendering)
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Tell OpenGL how to blend, in this case make the new object have the transparency of its alpha and the object in the back is 1-alpha

			// Draw the renderable 3d
			glm::mat4 model(1);
			model = glm::translate(model, current->getPosition());
			if ((current->getRotationAxis().x != 0 || current->getRotationAxis().y != 0 || current->getRotationAxis().z != 0) && current->getRadianRotation() != 0)
				model = glm::rotate(model, current->getRadianRotation(), current->getRotationAxis());
			model = glm::scale(model, current->getScale());
			shader.setUniformMat4("model", model);
			current->draw(shader);

			// Draw the outline
			if (current->getShouldOutline()) {
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

				outlineShader.enable();
				model = glm::mat4(1);
				model = glm::translate(model, current->getPosition());
				if ((current->getRotationAxis().x != 0 || current->getRotationAxis().y != 0 || current->getRotationAxis().z != 0) && current->getRadianRotation() != 0)
					model = glm::rotate(model, current->getRadianRotation(), current->getRotationAxis());
				model = glm::scale(model, current->getScale() + glm::vec3(0.025f, 0.025f, 0.025f));
				outlineShader.setUniformMat4("model", model);
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

} }