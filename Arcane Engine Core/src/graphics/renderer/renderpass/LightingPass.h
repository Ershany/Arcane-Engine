#pragma once

#include <graphics/renderer/renderpass/RenderPass.h>
#include <graphics/Shader.h>
#include <scene/Scene3D.h>

namespace arcane {

	class LightingPass : public RenderPass {
	public:
		LightingPass(Scene3D *scene, bool useIBL = true);
		LightingPass(Scene3D *scene, Framebuffer *customFramebuffer, bool useIBL = true);
		virtual ~LightingPass() override;

		LightingPassOutput executeRenderPass(ShadowmapPassOutput &shadowmapData, ICamera *camera);
	private:
		void bindShadowmap(Shader *shader, ShadowmapPassOutput &shadowmapData);
	private:
		Framebuffer *m_Framebuffer = nullptr;
		Shader *m_ModelShader, *m_TerrainShader;

		// Pass parameters
		bool m_UseIBL;
	};

}
