#pragma once

#include <graphics/renderer/renderpass/RenderPass.h>
#include <graphics/Shader.h>
#include <scene/Scene3D.h>

namespace arcane {

	class DeferredLightingPass : public RenderPass {
	public:
		DeferredLightingPass(Scene3D *scene);
		DeferredLightingPass(Scene3D *scene, Framebuffer *framebuffer);
		virtual ~DeferredLightingPass() override;

		LightingPassOutput executeLightingPass(ShadowmapPassOutput &shadowmapData, GeometryPassOutput &geometryData, PreLightingPassOutput &preLightingOutput, ICamera *camera, bool useIBL);
	private:
		void bindShadowmap(Shader *shader, ShadowmapPassOutput &shadowmapData);
	private:
		bool m_AllocatedFramebuffer;
		Framebuffer *m_Framebuffer;
		Shader *m_LightingShader;
	};
}
