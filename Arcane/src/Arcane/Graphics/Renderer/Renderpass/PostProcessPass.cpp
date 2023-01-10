#include "arcpch.h"
#include "PostProcessPass.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Scene/Scene3D.h>
#include <Arcane/UI/RuntimePane.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>
#include <Arcane/Util/Loaders/AssetManager.h>

namespace Arcane
{
	PostProcessPass::PostProcessPass(Scene3D *scene) : RenderPass(scene), m_SsaoRenderTarget((unsigned int)(Window::GetRenderResolutionWidth() * 0.5f), (unsigned int)(Window::GetRenderResolutionHeight() * 0.5f), false), m_SsaoBlurRenderTarget((unsigned int)(Window::GetRenderResolutionWidth() * 0.5f), (unsigned int)(Window::GetRenderResolutionHeight() * 0.5f), false),
		m_TonemappedNonLinearTarget(Window::GetWidth(), Window::GetHeight(), false), m_ScreenRenderTarget(Window::GetWidth(), Window::GetHeight(), false), m_ResolveRenderTarget(Window::GetRenderResolutionWidth(), Window::GetRenderResolutionHeight(), false), m_BrightPassRenderTarget(Window::GetWidth(), Window::GetHeight(), false),
		m_BloomFullRenderTarget(Window::GetWidth(), Window::GetHeight(), false), m_BloomHalfRenderTarget((unsigned int)(Window::GetWidth() * 0.5f), (unsigned int)(Window::GetHeight() * 0.5f), false), m_BloomQuarterRenderTarget((unsigned int)(Window::GetWidth() * 0.25f), (unsigned int)(Window::GetHeight() * 0.25f), false), m_BloomEightRenderTarget((unsigned int)(Window::GetWidth() * 0.125f), (unsigned int)(Window::GetHeight() * 0.125f), false),
		m_FullRenderTarget(Window::GetWidth(), Window::GetHeight(), false), m_HalfRenderTarget((unsigned int)(Window::GetWidth() * 0.5f), (unsigned int)(Window::GetHeight() * 0.5f), false), m_QuarterRenderTarget((unsigned int)(Window::GetWidth() * 0.25f), (unsigned int)(Window::GetWidth() * 0.25f), false), m_EightRenderTarget((unsigned int)(Window::GetWidth() * 0.125f), (unsigned int)(Window::GetHeight() * 0.125f), false),
		m_VignetteTexture(nullptr), m_SsaoNoiseTexture(), m_ProfilingTimer(), m_EffectsTimer()
	{
		// Shader setup
		m_TonemapGammaCorrectShader = ShaderLoader::LoadShader("TonemapGammaCorrect.glsl");
		m_FxaaShader = ShaderLoader::LoadShader("post_process/fxaa/FXAA.glsl");
		m_SsaoShader = ShaderLoader::LoadShader("post_process/ssao/SSAO.glsl");
		m_SsaoBlurShader = ShaderLoader::LoadShader("post_process/ssao/SSAO_Blur.glsl");
		m_BloomBrightPassShader = ShaderLoader::LoadShader("post_process/bloom/BloomBrightPass.glsl");
		m_BloomGaussianBlurShader = ShaderLoader::LoadShader("post_process/bloom/BloomGaussianBlur.glsl");
		m_BloomComposite = ShaderLoader::LoadShader("post_process/bloom/Composite.glsl");
		m_VignetteShader = ShaderLoader::LoadShader("post_process/vignette/vignette.glsl");
		m_ChromaticAberrationShader = ShaderLoader::LoadShader("post_process/chromatic_aberration/ChromaticAberration.glsl");
		m_FilmGrainShader = ShaderLoader::LoadShader("post_process/film_grain/FilmGrain.glsl");

		// Framebuffer setup
		m_SsaoRenderTarget.AddColorTexture(NormalizedSingleChannel8).CreateFramebuffer();
		m_SsaoBlurRenderTarget.AddColorTexture(NormalizedSingleChannel8).CreateFramebuffer();
		m_TonemappedNonLinearTarget.AddColorTexture(Normalized8).AddDepthStencilRBO(NormalizedDepthOnly).CreateFramebuffer();
		m_ScreenRenderTarget.AddColorTexture(FloatingPoint16).AddDepthStencilRBO(NormalizedDepthOnly).CreateFramebuffer();
		m_ResolveRenderTarget.AddColorTexture(FloatingPoint16).AddDepthStencilRBO(NormalizedDepthOnly).CreateFramebuffer();

		m_FullRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		m_HalfRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		m_QuarterRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		m_EightRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();

		m_BrightPassRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		m_BloomFullRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		m_BloomHalfRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		m_BloomQuarterRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		m_BloomEightRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();

		// SSAO Hemisphere Sample Generation (tangent space)
		std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
		std::default_random_engine generator;
		for (unsigned int i = 0; i < m_SsaoKernel.size(); i++) {
			// Make sure that the samples aren't perfectly perpendicular to the normal, or depth reconstruction will yield artifacts (so make sure the z value isn't close to 0)
			glm::vec3 hemisphereSample = glm::vec3((randomFloats(generator) * 2.0f) - 1.0f, (randomFloats(generator) * 2.0f) - 1.0f, glm::clamp((double)randomFloats(generator), 0.2, 1.0)); // Z = [0.2, 1] because we want hemisphere in tangent space
			hemisphereSample = glm::normalize(hemisphereSample);

			// Generate more samples closer to the origin of the hemisphere. Since these make for better light occlusion tests
			float scale = (float)i / m_SsaoKernel.size();
			scale = lerp(0.1f, 1.0f, scale * scale);
			hemisphereSample *= scale;

			m_SsaoKernel[i] = hemisphereSample;
		}

		// SSAO Random Rotation Texture (used to apply a random rotation when constructing the change of basis matrix)
		// Random vectors should be in tangent space
		std::array<glm::vec3, 16> noiseSSAO;
		for (unsigned int i = 0; i < noiseSSAO.size(); i++) {
			noiseSSAO[i] = glm::vec3((randomFloats(generator) * 2.0f) - 1.0f, (randomFloats(generator) * 2.0f) - 1.0f, 0.0f);
		}
		TextureSettings ssaoNoiseTextureSettings;
		ssaoNoiseTextureSettings.TextureFormat = GL_RGB16F;
		ssaoNoiseTextureSettings.TextureWrapSMode = GL_REPEAT;
		ssaoNoiseTextureSettings.TextureWrapTMode = GL_REPEAT;
		ssaoNoiseTextureSettings.TextureMinificationFilterMode = GL_NEAREST;
		ssaoNoiseTextureSettings.TextureMagnificationFilterMode = GL_NEAREST;
		ssaoNoiseTextureSettings.TextureAnisotropyLevel = 1.0f;
		ssaoNoiseTextureSettings.HasMips = false;
		m_SsaoNoiseTexture.SetTextureSettings(ssaoNoiseTextureSettings);
		m_SsaoNoiseTexture.Generate2DTexture(4, 4, GL_RGB, GL_FLOAT, &noiseSSAO[0]);
	}

	PostProcessPass::~PostProcessPass() {}

	// Generates the AO of the scene using SSAO and stores it in a single channel texture
	PreLightingPassOutput PostProcessPass::executePreLightingPass(GeometryPassOutput &geometryData, ICamera *camera) {
#if DEBUG_PROFILING
		glFinish();
		m_ProfilingTimer.Reset();
#endif // DEBUG_PROFILING
		PreLightingPassOutput passOutput;
		if (!m_SsaoEnabled) {
			passOutput.ssaoTexture = AssetManager::GetInstance().GetWhiteTexture();
			return passOutput;
		}

		// Generate the AO factors for the scene
		glViewport(0, 0, m_SsaoRenderTarget.GetWidth(), m_SsaoRenderTarget.GetHeight());
		m_SsaoRenderTarget.Bind();
		m_GLCache->SetDepthTest(false);
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetCullFace(GL_BACK);

		// Setup
		ModelRenderer *modelRenderer = m_ActiveScene->GetModelRenderer();

		// Bind the required data to perform SSAO
		m_GLCache->SetShader(m_SsaoShader);

		// Used to tile the noise texture across the screen every 4 texels (because our noise texture is 4x4)
		m_SsaoShader->SetUniform("noiseScale", glm::vec2(m_SsaoRenderTarget.GetWidth() * 0.25f, m_SsaoRenderTarget.GetHeight() * 0.25f));

		m_SsaoShader->SetUniform("ssaoStrength", m_SsaoStrength);
		m_SsaoShader->SetUniform("sampleRadius", m_SsaoSampleRadius);
		m_SsaoShader->SetUniform("sampleRadius2", m_SsaoSampleRadius * m_SsaoSampleRadius);
		m_SsaoShader->SetUniform("numKernelSamples", (int)m_SsaoKernel.size());
		m_SsaoShader->SetUniformArray("samples", static_cast<int>(m_SsaoKernel.size()), &m_SsaoKernel[0]);

		m_SsaoShader->SetUniform("view", camera->GetViewMatrix());
		m_SsaoShader->SetUniform("projection", camera->GetProjectionMatrix());
		m_SsaoShader->SetUniform("viewInverse", glm::inverse(camera->GetViewMatrix()));
		m_SsaoShader->SetUniform("projectionInverse", glm::inverse(camera->GetProjectionMatrix()));

		geometryData.outputGBuffer->GetNormal()->Bind(0);
		m_SsaoShader->SetUniform("normalTexture", 0);
		geometryData.outputGBuffer->GetDepthStencilTexture()->Bind(1);
		m_SsaoShader->SetUniform("depthTexture", 1);
		m_SsaoNoiseTexture.Bind(2);
		m_SsaoShader->SetUniform("texNoise", 2);

		// Render our NDC quad to perform SSAO
		modelRenderer->NDC_Plane.Draw();


		// Blur the result
		m_SsaoBlurRenderTarget.Bind();
		m_SsaoBlurShader->Enable();

		m_SsaoBlurShader->SetUniform("numSamplesAroundTexel", 2); // 5x5 kernel blur
		m_SsaoBlurShader->SetUniform("ssaoInput", 0); // Texture unit
		m_SsaoRenderTarget.GetColourTexture()->Bind(0);

		// Render our NDC quad to blur our SSAO texture
		modelRenderer->NDC_Plane.Draw();
		
		// Reset unusual state
		m_GLCache->SetDepthTest(true);

#if DEBUG_PROFILING
		glFinish();
		RuntimePane::SetSsaoTimer((float)m_ProfilingTimer.Elapsed());
#endif // DEBUG_PROFILING

		// Render pass output
		passOutput.ssaoTexture = m_SsaoBlurRenderTarget.GetColourTexture();
		return passOutput;
	}

	PostProcessPassOutput PostProcessPass::executePostProcessPass(Framebuffer *framebufferToProcess) {
		PostProcessPassOutput output;

		ModelRenderer *modelRenderer = m_ActiveScene->GetModelRenderer();
		GLCache *glCache = GLCache::GetInstance();

		// If the framebuffer is multi-sampled, resolve it
		Framebuffer *supersampledTarget = framebufferToProcess;
		if (framebufferToProcess->IsMultisampled()) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferToProcess->GetFramebuffer());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ResolveRenderTarget.GetFramebuffer());
			glBlitFramebuffer(0, 0, framebufferToProcess->GetWidth(), framebufferToProcess->GetHeight(), 0, 0, m_ResolveRenderTarget.GetWidth(), m_ResolveRenderTarget.GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
			supersampledTarget = &m_ResolveRenderTarget;
		}

		// If some sort of super-sampling is set, we need to downsample (or upsample) our image to match the window's resolution
		Framebuffer *inputFramebuffer = supersampledTarget;
		if (inputFramebuffer->GetWidth() != m_ScreenRenderTarget.GetWidth() || inputFramebuffer->GetHeight() != m_ScreenRenderTarget.GetHeight()) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, supersampledTarget->GetFramebuffer());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ScreenRenderTarget.GetFramebuffer());
			glBlitFramebuffer(0, 0, supersampledTarget->GetWidth(), supersampledTarget->GetHeight(), 0, 0, m_ScreenRenderTarget.GetWidth(), m_ScreenRenderTarget.GetHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
			inputFramebuffer = &m_ScreenRenderTarget;
		}

#ifdef ARC_DEV_BUILD
		if (DebugPane::GetWireframeMode())
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif // ARC_DEV_BUILD
		//Texture *sceneWithBloom = bloom(target->getColourTexture());
		//tonemapGammaCorrect(&m_TonemappedNonLinearTarget, sceneWithBloom);

		// Convert our scene from HDR (linear) -> SDR (sRGB)
		tonemapGammaCorrect(&m_TonemappedNonLinearTarget, inputFramebuffer->GetColourTexture());
		inputFramebuffer = &m_TonemappedNonLinearTarget;

		Framebuffer *framebufferToRenderTo = nullptr;
		if (m_ChromaticAberrationEnabled) {
			if (framebufferToRenderTo == &m_FullRenderTarget) framebufferToRenderTo = &m_TonemappedNonLinearTarget;
			else framebufferToRenderTo = &m_FullRenderTarget;

			chromaticAberration(framebufferToRenderTo, inputFramebuffer->GetColourTexture());
			inputFramebuffer = framebufferToRenderTo;
		}

		if (m_FilmGrainEnabled) {
			if (framebufferToRenderTo == &m_FullRenderTarget) framebufferToRenderTo = &m_TonemappedNonLinearTarget;
			else framebufferToRenderTo = &m_FullRenderTarget;

			filmGrain(framebufferToRenderTo, inputFramebuffer->GetColourTexture());
			inputFramebuffer = framebufferToRenderTo;
		}

		if (m_VignetteEnabled) {
			if (framebufferToRenderTo == &m_FullRenderTarget) framebufferToRenderTo = &m_TonemappedNonLinearTarget;
			else framebufferToRenderTo = &m_FullRenderTarget;

			vignette(framebufferToRenderTo, inputFramebuffer->GetColourTexture());
			inputFramebuffer = framebufferToRenderTo;
		}

#if DEBUG_PROFILING
		glFinish();
		m_ProfilingTimer.Reset();
#endif // DEBUG_PROFILING
		if (m_FxaaEnabled) {
			if (framebufferToRenderTo == &m_FullRenderTarget) framebufferToRenderTo = &m_TonemappedNonLinearTarget;
			else framebufferToRenderTo = &m_FullRenderTarget;

			fxaa(framebufferToRenderTo, inputFramebuffer->GetColourTexture());
			inputFramebuffer = framebufferToRenderTo;
		}
#if DEBUG_PROFILING
		glFinish();
		RuntimePane::SetFxaaTimer((float)m_ProfilingTimer.Elapsed());
#endif // DEBUG_PROFILING

		// Finally return the output frame after being post processed
		output.outFramebuffer = inputFramebuffer;
		return output;
	}

	void PostProcessPass::tonemapGammaCorrect(Framebuffer *target, Texture *hdrTexture) {
		glViewport(0, 0, target->GetWidth(), target->GetHeight());
		m_GLCache->SetShader(m_TonemapGammaCorrectShader);
		m_GLCache->SetDepthTest(false);
		m_GLCache->SetBlend(false);
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetCullFace(GL_BACK);
		m_GLCache->SetStencilTest(false);
		target->Bind();

		m_TonemapGammaCorrectShader->SetUniform("gamma_inverse", 1.0f / m_GammaCorrection); 
		m_TonemapGammaCorrectShader->SetUniform("exposure", m_Exposure);
		m_TonemapGammaCorrectShader->SetUniform("input_texture", 0);
		hdrTexture->Bind(0);

		m_ActiveScene->GetModelRenderer()->NDC_Plane.Draw();
	}

	void PostProcessPass::fxaa(Framebuffer *target, Texture *texture) {
		glViewport(0, 0, target->GetWidth(), target->GetHeight());
		m_GLCache->SetShader(m_FxaaShader);
		m_GLCache->SetDepthTest(false);
		m_GLCache->SetBlend(false);
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetCullFace(GL_BACK);
		m_GLCache->SetStencilTest(false);
		target->Bind();

		m_FxaaShader->SetUniform("texel_size", glm::vec2(1.0f / (float)texture->GetWidth(), 1.0f / (float)texture->GetHeight()));
		m_FxaaShader->SetUniform("input_texture", 0);
		texture->Bind(0);

		m_ActiveScene->GetModelRenderer()->NDC_Plane.Draw();
	}

	void PostProcessPass::vignette(Framebuffer *target, Texture *texture, Texture *optionalVignetteMask) {
		glViewport(0, 0, target->GetWidth(), target->GetHeight());
		m_GLCache->SetShader(m_VignetteShader);
		m_GLCache->SetDepthTest(false);
		m_GLCache->SetBlend(false);
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetCullFace(GL_BACK);
		m_GLCache->SetStencilTest(false);
		target->Bind();

		m_VignetteShader->SetUniform("colour", m_VignetteColour);
		m_VignetteShader->SetUniform("intensity", m_VignetteIntensity);
		m_VignetteShader->SetUniform("input_texture", 0);
		texture->Bind(0);
		if (optionalVignetteMask != nullptr) {
			m_VignetteShader->SetUniform("usesMask", 1);
			m_VignetteShader->SetUniform("vignette_mask", 1);
			optionalVignetteMask->Bind(1);
		}

		m_ActiveScene->GetModelRenderer()->NDC_Plane.Draw();
	}

	void PostProcessPass::chromaticAberration(Framebuffer *target, Texture *texture) {
		glViewport(0, 0, target->GetWidth(), target->GetHeight());
		m_GLCache->SetShader(m_ChromaticAberrationShader);
		m_GLCache->SetDepthTest(false);
		m_GLCache->SetBlend(false);
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetCullFace(GL_BACK);
		m_GLCache->SetStencilTest(false);
		target->Bind();

		m_ChromaticAberrationShader->SetUniform("intensity", m_ChromaticAberrationIntensity * 100);
		m_ChromaticAberrationShader->SetUniform("texel_size", glm::vec2(1.0f / (float)texture->GetWidth(), 1.0f / (float)texture->GetHeight()));
		m_ChromaticAberrationShader->SetUniform("input_texture", 0);
		texture->Bind(0);

		m_ActiveScene->GetModelRenderer()->NDC_Plane.Draw();
	}

	void PostProcessPass::filmGrain(Framebuffer *target, Texture *texture) {
		glViewport(0, 0, target->GetWidth(), target->GetHeight());
		m_GLCache->SetShader(m_FilmGrainShader);
		m_GLCache->SetDepthTest(false);
		m_GLCache->SetBlend(false);
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetCullFace(GL_BACK);
		m_GLCache->SetStencilTest(false);
		target->Bind();

		m_FilmGrainShader->SetUniform("intensity", m_FilmGrainIntensity * 100.0f);
		m_FilmGrainShader->SetUniform("time", (float)(std::fmod(m_EffectsTimer.Elapsed(), 100.0)));
		m_FilmGrainShader->SetUniform("input_texture", 0);
		texture->Bind(0);

		m_ActiveScene->GetModelRenderer()->NDC_Plane.Draw();
	}

	Texture* PostProcessPass::bloom(Texture *hdrSceneTexture) {
		m_GLCache->SetDepthTest(false);
		m_GLCache->SetBlend(false);
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetCullFace(GL_BACK);
		m_GLCache->SetStencilTest(false);

		// Bloom Bright Pass
		glViewport(0, 0, m_BrightPassRenderTarget.GetWidth(), m_BrightPassRenderTarget.GetHeight());
		m_BrightPassRenderTarget.Bind();
		m_BrightPassRenderTarget.Clear();
		m_GLCache->SetShader(m_BloomBrightPassShader);
		m_BloomBrightPassShader->SetUniform("threshold", m_BloomThreshold);
		m_BloomBrightPassShader->SetUniform("scene_capture", 0);
		hdrSceneTexture->Bind(0);
		m_ActiveScene->GetModelRenderer()->NDC_Plane.Draw();

		// Bloom Gaussian Blur Pass
		// As the render target gets smaller, we can increase the separable (two-pass) Gaussian kernel size
		m_GLCache->SetShader(m_BloomGaussianBlurShader);
		glViewport(0, 0, m_FullRenderTarget.GetWidth(), m_FullRenderTarget.GetHeight());
		m_FullRenderTarget.Bind();
		m_FullRenderTarget.Clear();
		m_BloomGaussianBlurShader->SetUniform("isVerticalBlur", true);
		m_BloomGaussianBlurShader->SetUniform("read_offset", glm::vec2(1.0f / (float)m_FullRenderTarget.GetWidth(), 1.0f / (float)m_FullRenderTarget.GetHeight()));
		m_BloomGaussianBlurShader->SetUniform("bloom_texture", 0);
		m_BrightPassRenderTarget.GetColourTexture()->Bind(0);
		m_ActiveScene->GetModelRenderer()->NDC_Plane.Draw();

		m_BloomFullRenderTarget.Bind();
		m_BloomFullRenderTarget.Clear();
		m_BloomGaussianBlurShader->SetUniform("isVerticalBlur", false);
		m_BloomGaussianBlurShader->SetUniform("read_offset", glm::vec2(1.0f / (float)m_BloomFullRenderTarget.GetWidth(), 1.0f / (float)m_BloomFullRenderTarget.GetHeight()));
		m_BloomGaussianBlurShader->SetUniform("bloom_texture", 0);
		m_FullRenderTarget.GetColourTexture()->Bind(0);
		m_ActiveScene->GetModelRenderer()->NDC_Plane.Draw();

		// Combine our bloom texture with the scene
		m_GLCache->SetShader(m_BloomComposite);
		glViewport(0, 0, m_FullRenderTarget.GetWidth(), m_FullRenderTarget.GetHeight());
		m_FullRenderTarget.Bind();
		m_BloomComposite->SetUniform("strength", 1.0f);
		m_BloomComposite->SetUniform("scene_texture", 0);
		m_BloomComposite->SetUniform("bloom_texture", 1);
		hdrSceneTexture->Bind(0);
		m_BloomFullRenderTarget.GetColourTexture()->Bind(1);
		m_ActiveScene->GetModelRenderer()->NDC_Plane.Draw();

		return m_FullRenderTarget.GetColourTexture();
	}
}
