#pragma once
#ifndef DEFERREDGEOMETRYPASS_H
#define DEFERREDGEOMETRYPASS_H

#ifndef RENDERPASS_H
#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>
#endif

#ifndef RENDERPASSTYPE_H
#include <Arcane/Graphics/Renderer/Renderpass/RenderPassType.h>
#endif

namespace Arcane
{
	class Shader;
	class Scene;
	class ICamera;
	class GBuffer;

	class DeferredGeometryPass : public RenderPass {
	public:
		DeferredGeometryPass(Scene *scene);
		DeferredGeometryPass(Scene *scene, GBuffer *customGBuffer);
		virtual ~DeferredGeometryPass() override;

		GeometryPassOutput ExecuteGeometryPass(ICamera *camera, bool renderOnlyStatic);
	private:
		bool m_AllocatedGBuffer;
		GBuffer *m_GBuffer;
		Shader *m_ModelShader, *m_SkinnedModelShader, *m_TerrainShader;
	};
}
#endif
