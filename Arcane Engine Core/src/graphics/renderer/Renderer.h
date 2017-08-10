#pragma once

#include "../Model.h"
#include "Renderable3D.h"
#include <deque>

namespace arcane { namespace graphics {
	class Renderer {
	public:
		Renderer();

		void submitOpaque(Renderable3D *renderable);
		void submitTransparent(Renderable3D *renderable);
		
		void flush(Shader &shader, Shader &outlineShader);
	private:
		std::deque<Renderable3D*> m_OpaqueRenderQueue;
		std::deque<Renderable3D*> m_TransparentRenderQueue;
	};
} }