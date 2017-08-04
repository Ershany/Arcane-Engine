#include "Renderer.h"

namespace arcane { namespace graphics {

	Renderer::Renderer()
	{
	}

	void Renderer::submit(Renderable3D *renderable) {
		m_RenderQueue.push_back(renderable);
	}

	void Renderer::flush(Shader &shader, Shader &outlineShader) {
		while (!m_RenderQueue.empty()) {
			Renderable3D *current = m_RenderQueue.front();

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
			}
			glClear(GL_STENCIL_BUFFER_BIT);

			m_RenderQueue.pop_front();
		}
	}

} }