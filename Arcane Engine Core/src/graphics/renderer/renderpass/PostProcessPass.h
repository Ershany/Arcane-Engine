#pragma once

#include <graphics/renderer/renderpass/RenderPass.h>
#include <graphics/Shader.h>
#include <scene/Scene3D.h>
#include <utils/Timer.h>

namespace arcane {

	class PostProcessPass : public RenderPass {
	public:
		PostProcessPass(Scene3D *scene);
		virtual ~PostProcessPass() override;

		PreLightingPassOutput executePreLightingPass(GeometryPassOutput &geometryData, ICamera *camera);
		void executePostProcessPass(Framebuffer *framebufferToProcess);
	private:
		inline float lerp(float a, float b, float amount) { return a + amount * (b - a); }
	private:
		Shader *m_PostProcessShader;
		Shader *m_FxaaShader;
		Shader *m_SsaoShader, *m_SsaoBlurShader;

		Framebuffer m_SsaoRenderTarget;
		Framebuffer m_SsaoBlurRenderTarget;
		Framebuffer m_TonemappedNonLinearTarget;
		Framebuffer m_ScreenRenderTarget; // Only used if the render resolution differs from the window resolution
		Framebuffer m_ResolveRenderTarget; // Only used if multi-sampling is enabled so it can be resolved

		Texture m_ScreenTexture; // Used to copy the scene's colour buffer and then glGenerateMipmaps is called so we can get the average luminance of the scene

		// Post Processing Tweaks
		float m_GammaCorrection = 2.2f;
		float m_Exposure = 1.0f;
		bool m_FxaaEnabled = true;
		bool m_SsaoEnabled = true;
		float m_SsaoSampleRadius = 2.0f;
		float m_SsaoStrength = 3.0f;

		// SSAO Tweaks
		std::array<glm::vec3, SSAO_KERNEL_SIZE> m_SsaoKernel;
		Texture m_SsaoNoiseTexture;

		Timer m_Timer;
	};

}
