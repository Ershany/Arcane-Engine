#pragma once

#include <deque>
#include <glm\gtx\norm.hpp>

#include "../mesh/Model.h"
#include "../camera/Camera.h"
#include "Renderable3D.h"
#include "GLCache.h"

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

		// TODO: ADD QUAD TYPE - GOES HERE CALLE m_NDCPLane

		Camera *m_Camera;
		GLCache *m_GLCache;
	};
} }