#include "Renderer.h"

namespace arcane { namespace graphics {

	void Renderer::init() {

	}

	void Renderer::submit(Renderable3D *renderable) {
		m_RenderQueue.push_back(renderable);
	}

	void Renderer::flush(Shader &shader) {
		while (!m_RenderQueue.empty()) {
			m_RenderQueue.front()->draw(shader);

			m_RenderQueue.pop_front();
		}
	}

} }