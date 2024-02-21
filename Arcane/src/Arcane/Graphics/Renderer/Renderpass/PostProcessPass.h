#pragma once
#ifndef POSTPROCESSPASS_H
#define POSTPROCESSPASS_H

#ifndef RENDERPASS_H
#include <Arcane/Graphics/Renderer/Renderpass/RenderPass.h>
#endif

#ifndef FRAMEBUFFER_H
#include <Arcane/Platform/OpenGL/Framebuffer/Framebuffer.h>
#endif

#ifndef TIMER_H
#include <Arcane/Util/Timer.h>
#endif

#ifndef RENDERPASSTYPE_H
#include <Arcane/Graphics/Renderer/Renderpass/RenderPassType.h>
#endif

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

		// Bloom bindings
		inline bool& GetBloomEnabledRef() { return m_BloomEnabled; }
		inline float& GetBloomThresholdRef() { return m_BloomThreshold; }
		inline float& GetBloomSoftThresholdRef() { return m_BloomSoftThreshold; }
		inline float& GetBloomStrengthRef() { return m_BloomStrength; }
		inline Texture* GetBloomDirtTexture() { return m_BloomDirtTexture; }
		inline float& GetBloomDirtMaskIntensityRef() { return m_BloomDirtMaskIntensity; }

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
		// Silly to manage all of these like this
		inline Framebuffer* GetFullRenderTarget() { return &m_FullRenderTarget; }
		inline Framebuffer* GetHalfRenderTarget() { return &m_HalfRenderTarget; }
		inline Framebuffer* GetQuarterRenderTarget() { return &m_QuarterRenderTarget; }
		inline Framebuffer* GetEighthRenderTarget() { return &m_EighthRenderTarget; }
		inline Framebuffer* GetResolveRenderTarget() { return &m_ResolveRenderTarget; }
		inline Framebuffer* GetTonemappedNonLinearTarget() { return &m_TonemappedNonLinearTarget; }

		// Bloom settings
		inline void SetBloomDirtTexture(Texture *texture) { m_BloomDirtTexture = texture; }
		inline void SetBloomDirtMaskIntensity(float intensity) { m_BloomDirtMaskIntensity = intensity; }

		// Vignette settings
		inline void SetVignetteTexture(Texture *texture) { m_VignetteTexture = texture; }
	private:
		inline float Lerp(float a, float b, float amount) { return a + amount * (b - a); }
	private:
		Shader *m_TonemapGammaCorrectShader;
		Shader *m_FxaaShader;
		Shader *m_SsaoShader, *m_SsaoBlurShader;
		Shader *m_BloomBrightPassShader, *m_BloomDownsampleShader, *m_BloomUpsampleShader, *m_BloomCompositeShader;
		Shader *m_VignetteShader;
		Shader *m_ChromaticAberrationShader;
		Shader *m_FilmGrainShader;

		Framebuffer m_SsaoRenderTarget;
		Framebuffer m_SsaoBlurRenderTarget;
		Framebuffer m_TonemappedNonLinearTarget;
		Framebuffer m_ResolveRenderTarget; // Only used if multi-sampling is enabled so it can be resolved

		Texture *m_BloomDirtTexture = nullptr;
		Framebuffer m_BrightPassRenderTarget;
		Framebuffer m_BloomHalfRenderTarget;
		Framebuffer m_BloomQuarterRenderTarget;
		Framebuffer m_BloomEightRenderTarget;
		Framebuffer m_BloomSixteenRenderTarget;
		Framebuffer m_BloomThirtyTwoRenderTarget;
		Framebuffer m_BloomSixtyFourRenderTarget;

		// Utility Framebuffers
		Framebuffer m_FullRenderTarget;
		Framebuffer m_HalfRenderTarget;
		Framebuffer m_QuarterRenderTarget;
		Framebuffer m_EighthRenderTarget;

		// Post Processing Tweaks
		float m_GammaCorrection = 2.2f;
		float m_Exposure = 1.0f;
		bool m_BloomEnabled = true;
		float m_BloomThreshold = 3.0f;
		float m_BloomSoftThreshold = 0.5f; // [0, 1] 0 = hard cutoff, 1 = soft cutoff between bloom vs no bloom
		float m_BloomStrength = 0.4f;
		float m_BloomDirtMaskIntensity = 5.0f;
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

		Timer m_EffectsTimer;
	};
}
#endif
