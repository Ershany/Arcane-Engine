#pragma once

#include <deque>
#include <glm\gtx\norm.hpp>

#include "../mesh/Model.h"
#include "../camera/FPSCamera.h"
#include "Renderable3D.h"
#include "GLCache.h"
#include "RenderPass.h"
#include "../mesh/common/Quad.h"

namespace arcane { namespace graphics {

	class MeshRenderer {
	public:
		MeshRenderer(FPSCamera *camera);

		void submitOpaque(Renderable3D *renderable);
		void submitTransparent(Renderable3D *renderable);
		
		void flushOpaque(Shader &shader, RenderPass pass);
		void flushTransparent(Shader &shader, RenderPass pass);
	public:
		Quad NDC_Plane;
	private:
		void MeshRenderer::setupModelMatrix(Renderable3D *renderable, Shader &shader, RenderPass pass);

		std::deque<Renderable3D*> m_OpaqueRenderQueue;
		std::deque<Renderable3D*> m_TransparentRenderQueue;

		FPSCamera *m_Camera;
		GLCache *m_GLCache;
	};
} }