#pragma once

#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>

namespace Arcane
{
	class Shader;
	class Scene;
	class ICamera;

	class DeferredGeometryPass : public RenderPass {
	public:
		DeferredGeometryPass(Scene *scene);
		DeferredGeometryPass(Scene *scene, GBuffer *customGBuffer);
		virtual ~DeferredGeometryPass() override;

		GeometryPassOutput ExecuteGeometryPass(ICamera *camera, bool renderOnlyStatic);
	private:
		bool m_AllocatedGBuffer;
		GBuffer *m_GBuffer;
		Shader *m_ModelShader, *m_TerrainShader;
	};
}
