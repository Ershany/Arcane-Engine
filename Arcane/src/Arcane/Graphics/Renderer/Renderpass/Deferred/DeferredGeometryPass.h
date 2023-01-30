#pragma once

#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>

namespace Arcane
{
	class Shader;
	class Scene;
	class ICamera;

	enum DeferredStencilValue : int {
		ModelStencilValue = 0x01,
		TerrainStencilValue = 0x02
	};

	class DeferredGeometryPass : public RenderPass {
	public:
		DeferredGeometryPass(Scene *scene);
		DeferredGeometryPass(Scene *scene, GBuffer *customGBuffer);
		virtual ~DeferredGeometryPass() override;

		GeometryPassOutput executeGeometryPass(ICamera *camera, bool renderOnlyStatic);
	private:
		bool m_AllocatedGBuffer;
		GBuffer *m_GBuffer;
		Shader *m_ModelShader, *m_TerrainShader;
	};
}
