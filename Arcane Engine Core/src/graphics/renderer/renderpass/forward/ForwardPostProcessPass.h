#pragma once

#include <graphics/renderer/renderpass/RenderPass.h>
#include <graphics/Shader.h>
#include <scene/Scene3D.h>

namespace arcane {

	class ForwardPostProcessPass : public RenderPass {
	public:
		ForwardPostProcessPass(Scene3D *scene);
		virtual ~ForwardPostProcessPass() override;

		void executePostLightingPass(Framebuffer *framebufferToProcess);

		inline void EnableBlur(bool choice) { m_Blur = choice; }
	private:
		Shader *m_PostProcessShader;
		Framebuffer m_ScreenRenderTarget; // Only used if multi-sampling is enabled so it can blit to a non-multisampled buffer

		// Post Processing Tweaks
		float m_GammaCorrection = 2.2f;
		bool m_Blur = false;
	};

}
