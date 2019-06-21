#pragma once

#include <graphics/renderer/renderpass/RenderPass.h>
#include <graphics/Shader.h>
#include <scene/Scene3D.h>

namespace arcane {

	class GeometryPass : public RenderPass {
	public:
		GeometryPass(Scene3D *scene);
		GeometryPass(Scene3D *scene, GBuffer *customGBuffer);
		virtual ~GeometryPass() override;

		GeometryPassOutput executeRenderPass(ICamera *camera, bool renderOnlyStatic);
	private:
		bool m_AllocatedGBuffer;
		GBuffer *m_GBuffer;
		Shader *m_ModelShader, *m_TerrainShader;
	};

}
