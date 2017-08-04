#pragma once

#include "../Model.h"
#include "Renderable3D.h"
#include <deque>

namespace arcane { namespace graphics {
	class Renderer {
	public:
		Renderer();

		void submit(Renderable3D *renderable);
		void flush(Shader &shader, Shader &outlineShader);
	private:
		std::deque<Renderable3D*> m_RenderQueue;
	};
} }