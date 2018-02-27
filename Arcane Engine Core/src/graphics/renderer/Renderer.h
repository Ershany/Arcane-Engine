#pragma once

#include "../Model.h"
#include "../camera/Camera.h"
#include "Renderable3D.h"
#include <deque>
#include <glm\gtx\norm.hpp>

namespace arcane { namespace graphics {
	class Renderer {
	public:
		Renderer(Camera *camera);

		void submitOpaque(Renderable3D *renderable);
		void submitTransparent(Renderable3D *renderable);
		
		void flushOpaque(Shader &shader, Shader &outlineShader);
		void flushTransparent(Shader &shader, Shader &outlineShader);
	private:
		void Renderer::setupModelMatrix(Renderable3D *renderable, Shader &shader, float scaleFactor = 1.0f);
		void drawOutline(Shader &outlineShader, Renderable3D *renderable);

		std::deque<Renderable3D*> m_OpaqueRenderQueue;
		std::deque<Renderable3D*> m_TransparentRenderQueue;

		Camera *m_Camera;
	};
} }