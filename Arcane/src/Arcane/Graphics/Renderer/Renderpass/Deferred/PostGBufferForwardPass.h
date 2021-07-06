#pragma once

#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>

namespace Arcane
{
	class Shader;
	class Scene3D;
	class ICamera;

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
