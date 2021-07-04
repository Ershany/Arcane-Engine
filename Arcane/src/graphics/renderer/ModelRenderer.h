#pragma once

#include "GLCache.h"

#include <scene/RenderableModel.h>
#include <graphics/camera/FPSCamera.h>
#include <graphics/mesh/Model.h>
#include <graphics/mesh/common/Quad.h>
#include <graphics/mesh/common/Cube.h>
#include <graphics/renderer/renderpass/RenderPassType.h>

namespace Arcane
{
	class ModelRenderer {
	public:
		ModelRenderer(FPSCamera *camera);

		void SubmitOpaque(RenderableModel *renderable);
		void SubmitTransparent(RenderableModel *renderable);
		
		void SetupOpaqueRenderState();
		void SetupTransparentRenderState();

		void FlushOpaque(Shader *shader, RenderPassType pass);
		void FlushTransparent(Shader *shader, RenderPassType pass);
	public:
		Quad NDC_Plane;
		Cube NDC_Cube;
	private:
		void ModelRenderer::SetupModelMatrix(RenderableModel *renderable, Shader *shader, RenderPassType pass);
	private:
		std::deque<RenderableModel*> m_OpaqueRenderQueue;
		std::deque<RenderableModel*> m_TransparentRenderQueue;

		FPSCamera *m_Camera;
		GLCache *m_GLCache;
	};
}
