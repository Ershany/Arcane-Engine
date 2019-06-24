#pragma once

#include <graphics/renderer/renderpass/RenderPass.h>
#include <graphics/Shader.h>
#include <scene/Scene3D.h>

namespace arcane {

	class DeferredPostProcessPass : public RenderPass {
	public:
		DeferredPostProcessPass(Scene3D *scene);
		virtual ~DeferredPostProcessPass() override;

		void executePreLightingPass(Framebuffer *framebufferToProcess);
		void executePostLightingPass(Framebuffer *framebufferToProcess);

		inline void EnableBlur(bool choice) { m_Blur = choice; }
	private:
		// Downsample
		// Two Pass Blur
	private:
		Shader *m_PostProcessShader;

		// Post Processing Tweaks
		float m_GammaCorrection = 2.2f;
		bool m_Blur = false;
	};

}
