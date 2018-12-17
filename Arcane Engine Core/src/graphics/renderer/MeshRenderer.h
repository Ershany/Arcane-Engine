#pragma once

#include "GLCache.h"
#include "RenderPass.h"

#include <scene/RenderableModel.h>
#include <graphics/camera/FPSCamera.h>
#include <graphics/mesh/Model.h>
#include <graphics/mesh/common/Quad.h>

namespace arcane {

	class MeshRenderer {
	public:
		MeshRenderer(FPSCamera *camera);

		void submitOpaque(RenderableModel *renderable);
		void submitTransparent(RenderableModel *renderable);
		
		void flushOpaque(Shader &shader, RenderPass pass);
		void flushTransparent(Shader &shader, RenderPass pass);
	public:
		Quad NDC_Plane;
	private:
		void MeshRenderer::setupModelMatrix(RenderableModel *renderable, Shader &shader, RenderPass pass);

		std::deque<RenderableModel*> m_OpaqueRenderQueue;
		std::deque<RenderableModel*> m_TransparentRenderQueue;

		FPSCamera *m_Camera;
		GLCache *m_GLCache;
	};

}
