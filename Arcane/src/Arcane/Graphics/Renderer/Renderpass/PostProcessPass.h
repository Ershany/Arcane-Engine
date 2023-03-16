#pragma once

#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>
#include <Arcane/Platform/OpenGL/Framebuffer/Framebuffer.h>
#include <Arcane/Util/Timer.h>

namespace Arcane
{
	class Shader;
	class Scene;
	class ICamera;

	class PostProcessPass : public RenderPass {
	public:
		PostProcessPass(Scene *scene);
		virtual ~PostProcessPass() override;

		PreLightingPassOutput ExecutePreLightingPass(GBuffer *inputGbuffer, ICamera *camera);
		PostProcessPassOutput ExecutePostProcessPass(Framebuffer *framebufferToProcess);

		// Post Processing Effects
		void TonemapGammaCorrect(Framebuffer *target, Texture *hdrTexture);
		void Fxaa(Framebuffer *target, Texture *texture);
		void Vignette(Framebuffer *target, Texture *texture, Texture *optionalVignetteMask = nullptr);
		void ChromaticAberration(Framebuffer *target, Texture *texture);
		void FilmGrain(Framebuffer *target, Texture *texture);
		Texture* Bloom(Texture *hdrSceneTexture);

		// Tonemap bindings
		inline float& GetGammaCorrectionRef() { return m_GammaCorrection; }
		inline float& GetExposureRef() { return m_Exposure; }

		// SSAO bindings
		inline bool& GetSsaoEnabledRef() { return m_SsaoEnabled; }
		inline float& GetSsaoSampleRadiusRef() { return m_SsaoSampleRadius; }
		inline float& GetSsaoStrengthRef() { return m_SsaoStrength; }

		// FXAA bindings
		inline bool& GetFxaaEnabledRef() { return m_FxaaEnabled; }

		// Vignette bindings
		inline bool& GetVignetteEnabledRef() { return m_VignetteEnabled; }
		inline Texture* GetVignetteTexture() { return m_VignetteTexture; }
		inline glm::vec3& GetVignetteColourRef() { return m_VignetteColour; }
		inline float& GetVignetteIntensityRef() { return m_VignetteIntensity; }

		// Chromatic Aberration bindings
		inline bool& GetChromaticAberrationEnabledRef() { return m_ChromaticAberrationEnabled; }
		inline float& GetChromaticAberrationIntensityRef() { return m_ChromaticAberrationIntensity; }

		// Film Grain bindings
		inline bool& GetFilmGrainEnabledRef() { return m_FilmGrainEnabled; }
		inline float& GetFilmGrainIntensityRef() { return m_FilmGrainIntensity; }

		// Render Target Access (TODO: Should use render target aliasing and have a system for sharing render targets for different render passes. But this will suffice for now)
		inline Framebuffer* GetFullRenderTarget() { return &m_FullRenderTarget; }
		inline Framebuffer* GetHalfRenderTarget() { return &m_HalfRenderTarget; }
		inline Framebuffer* GetQuarterRenderTarget() { return &m_QuarterRenderTarget; }
		inline Framebuffer* GetEighthRenderTarget() { return &m_EighthRenderTarget; }
	private:
		inline float Lerp(float a, float b, float amount) { return a + amount * (b - a); }
	private:
		Shader *m_TonemapGammaCorrectShader;
		Shader *m_FxaaShader;
		Shader *m_SsaoShader, *m_SsaoBlurShader;
		Shader *m_BloomBrightPassShader, *m_BloomGaussianBlurShader, *m_BloomComposite;
		Shader *m_VignetteShader;
		Shader *m_ChromaticAberrationShader;
		Shader *m_FilmGrainShader;

		Framebuffer m_SsaoRenderTarget;
		Framebuffer m_SsaoBlurRenderTarget;
		Framebuffer m_TonemappedNonLinearTarget;
		Framebuffer m_ScreenRenderTarget; // Only used if the render resolution differs from the window resolution
		Framebuffer m_ResolveRenderTarget; // Only used if multi-sampling is enabled so it can be resolved

		Framebuffer m_BrightPassRenderTarget;
		Framebuffer m_BloomFullRenderTarget;
		Framebuffer m_BloomHalfRenderTarget;
		Framebuffer m_BloomQuarterRenderTarget;
		Framebuffer m_BloomEightRenderTarget;

		// Utility Framebuffers
		Framebuffer m_FullRenderTarget;
		Framebuffer m_HalfRenderTarget;
		Framebuffer m_QuarterRenderTarget;
		Framebuffer m_EighthRenderTarget;

		// Post Processing Tweaks
		float m_GammaCorrection = 2.2f;
		float m_Exposure = 1.0f;
		float m_BloomThreshold = 1.0f;
		bool m_FxaaEnabled = true;
		bool m_SsaoEnabled = true;
		float m_SsaoSampleRadius = 2.0f;
		float m_SsaoStrength = 3.0f;
		bool m_VignetteEnabled = false;
		Texture *m_VignetteTexture;
		glm::vec3 m_VignetteColour = glm::vec3(0.0f, 0.0f, 0.0f);
		float m_VignetteIntensity = 0.25f;
		bool m_ChromaticAberrationEnabled = false;
		float m_ChromaticAberrationIntensity = 0.25f;
		bool m_FilmGrainEnabled = false;
		float m_FilmGrainIntensity = 0.25f;

		// SSAO Tweaks
		std::array<glm::vec3, SSAO_KERNEL_SIZE> m_SsaoKernel;
		Texture m_SsaoNoiseTexture;

		Timer m_ProfilingTimer;
		Timer m_EffectsTimer;
	};
}
