#pragma once

#include <graphics/renderer/renderpass/RenderPass.h>
#include <graphics/Shader.h>
#include <scene/Scene3D.h>

namespace arcane {

	class PostGBufferForward : public RenderPass {
	public:
		PostGBufferForward(Scene3D *scene);
		virtual ~PostGBufferForward() override;

		LightingPassOutput executeLightingPass(ShadowmapPassOutput &shadowmapData, LightingPassOutput &lightingPassData, ICamera *camera, bool renderOnlyStatic, bool useIBL);
	private:
		void bindShadowmap(Shader *shader, ShadowmapPassOutput &shadowmapData);
	private:
		Shader *m_ModelShader;
	};

}
