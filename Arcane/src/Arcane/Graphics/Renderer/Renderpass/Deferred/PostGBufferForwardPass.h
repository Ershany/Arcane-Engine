#pragma once

#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>

namespace Arcane
{
	class Shader;
	class Scene;
	class ICamera;

	class PostGBufferForward : public RenderPass {
	public:
		PostGBufferForward(Scene *scene);
		virtual ~PostGBufferForward() override;

		LightingPassOutput executeLightingPass(ShadowmapPassOutput &inputShadowmapData, Framebuffer *inputFramebuffer, ICamera *camera, bool renderOnlyStatic, bool useIBL);
	private:
		void bindShadowmap(Shader *shader, ShadowmapPassOutput &shadowmapData);
	private:
		Shader *m_ModelShader;
	};
}
