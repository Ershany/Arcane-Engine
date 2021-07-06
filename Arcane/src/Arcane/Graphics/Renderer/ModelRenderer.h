#pragma once

#include <Arcane/Scene/RenderableModel.h>
#include <Arcane/Graphics/Mesh/Model.h>
#include <Arcane/Graphics/Mesh/Common/Quad.h>
#include <Arcane/Graphics/Mesh/Common/Cube.h>
#include <Arcane/Graphics/Renderer/Renderpass/RenderPassType.h>

namespace Arcane
{
	class GLCache;
	class FPSCamera;

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
