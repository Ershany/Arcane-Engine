#pragma once

#include "../Model.h"
#include "../camera/FPSCamera.h"
#include "Renderable3D.h"
#include <deque>
#include <glm\gtx\norm.hpp>

namespace arcane { namespace graphics {
	class Renderer {
	public:
		Renderer(FPSCamera *camera);

		void submitOpaque(Renderable3D *renderable);
		void submitTransparent(Renderable3D *renderable);
		
		void flushOpaque(Shader &shader, Shader &outlineShader);
		void flushTransparent(Shader &shader, Shader &outlineShader);
	private:
		std::deque<Renderable3D*> m_OpaqueRenderQueue;
		std::deque<Renderable3D*> m_TransparentRenderQueue;

		FPSCamera *m_Camera;
	};
} }