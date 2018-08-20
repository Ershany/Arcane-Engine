#pragma once

#include <deque>
#include <glm\gtx\norm.hpp>

#include "../mesh/Model.h"
#include "../camera/Camera.h"
#include "Renderable3D.h"
#include "GLCache.h"
#include "RenderPass.h"

namespace arcane { namespace graphics {

	class Renderer {
	public:
		Renderer(Camera *camera);

		void submitOpaque(Renderable3D *renderable);
		void submitTransparent(Renderable3D *renderable);
		
		void flushOpaque(Shader &shader, RenderPass pass);
		void flushTransparent(Shader &shader, RenderPass pass);
	private:
		void Renderer::setupModelMatrix(Renderable3D *renderable, Shader &shader, RenderPass pass);

		std::deque<Renderable3D*> m_OpaqueRenderQueue;
		std::deque<Renderable3D*> m_TransparentRenderQueue;

		// TODO: ADD QUAD TYPE - GOES HERE CALLED m_NDCPLane

		Camera *m_Camera;
		GLCache *m_GLCache;
	};
} }