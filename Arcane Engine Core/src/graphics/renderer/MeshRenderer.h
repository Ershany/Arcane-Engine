#pragma once

#include "GLCache.h"
#include "RenderPass.h"

#include <scene/Renderable3D.h>
#include <graphics/camera/FPSCamera.h>
#include <graphics/mesh/Model.h>
#include <graphics/mesh/common/Quad.h>

namespace arcane {

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

}
