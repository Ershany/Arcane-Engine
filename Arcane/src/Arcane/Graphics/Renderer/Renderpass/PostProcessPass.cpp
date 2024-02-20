#include "arcpch.h"
#include "PostProcessPass.h"

#include <Arcane/Core/Application.h>
#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Scene/Scene.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>
#include <Arcane/Util/Loaders/AssetManager.h>

namespace Arcane
{
	PostProcessPass::PostProcessPass(Scene *scene) : RenderPass(scene), m_SsaoRenderTarget((unsigned int)(Window::GetRenderResolutionWidth() * 0.5f), (unsigned int)(Window::GetRenderResolutionHeight() * 0.5f), false), m_SsaoBlurRenderTarget((unsigned int)(Window::GetRenderResolutionWidth() * 0.5f), (unsigned int)(Window::GetRenderResolutionHeight() * 0.5f), false),
		m_TonemappedNonLinearTarget(Window::GetRenderResolutionWidth(), Window::GetRenderResolutionHeight(), false), m_ResolveRenderTarget(Window::GetRenderResolutionWidth(), Window::GetRenderResolutionHeight(), false), m_BrightPassRenderTarget(Window::GetRenderResolutionWidth(), Window::GetRenderResolutionHeight(), false),
		m_BloomHalfRenderTarget((unsigned int)(Window::GetRenderResolutionWidth() * (1.0f / 2.0f)), (unsigned int)(Window::GetRenderResolutionHeight() * (1.0f / 2.0f)), false), m_BloomQuarterRenderTarget((unsigned int)(Window::GetRenderResolutionWidth() * (1.0f / 4.0f)), (unsigned int)(Window::GetRenderResolutionHeight() * (1.0f / 4.0f)), false), m_BloomEightRenderTarget((unsigned int)(Window::GetRenderResolutionWidth() * (1.0f / 8.0f)), (unsigned int)(Window::GetRenderResolutionHeight() * (1.0f / 8.0f)), false),
		m_BloomSixteenRenderTarget((unsigned int)(Window::GetRenderResolutionWidth() * (1.0f / 16.0f)), (unsigned int)(Window::GetRenderResolutionHeight() * (1.0f / 16.0f)), false), m_BloomThirtyTwoRenderTarget((unsigned int)(Window::GetRenderResolutionWidth() * (1.0f / 32.0f)), (unsigned int)(Window::GetRenderResolutionHeight() * (1.0f / 32.0f)), false), m_BloomSixtyFourRenderTarget((unsigned int)(Window::GetRenderResolutionWidth() * (1.0f / 64.0f)), (unsigned int)(Window::GetRenderResolutionHeight() * (1.0f / 64.0f)), false),
		m_FullRenderTarget(Window::GetRenderResolutionWidth(), Window::GetRenderResolutionHeight(), false), m_HalfRenderTarget((unsigned int)(Window::GetRenderResolutionWidth() * (1.0f / 2.0f)), (unsigned int)(Window::GetRenderResolutionHeight() * (1.0f / 2.0f)), false), m_QuarterRenderTarget((unsigned int)(Window::GetRenderResolutionWidth() * (1.0f / 4.0f)), (unsigned int)(Window::GetRenderResolutionHeight() * (1.0f / 4.0f)), false), m_EighthRenderTarget((unsigned int)(Window::GetRenderResolutionWidth() * (1.0f / 8.0f)), (unsigned int)(Window::GetRenderResolutionHeight() * (1.0f / 8.0f)), false),
		m_VignetteTexture(nullptr), m_SsaoNoiseTexture(), m_EffectsTimer()
	{
		ARC_ASSERT(m_BloomSixtyFourRenderTarget.GetWidth() >= 1 && m_BloomSixtyFourRenderTarget.GetHeight() >= 1, "Render resolution is too low for bloom");

		// Shader setup
		m_TonemapGammaCorrectShader = ShaderLoader::LoadShader("TonemapGammaCorrect.glsl");
		m_FxaaShader = ShaderLoader::LoadShader("post_process/fxaa/FXAA.glsl");
		m_SsaoShader = ShaderLoader::LoadShader("post_process/ssao/SSAO.glsl");
		m_SsaoBlurShader = ShaderLoader::LoadShader("post_process/ssao/SSAO_Blur.glsl");
		m_BloomBrightPassShader = ShaderLoader::LoadShader("post_process/bloom/BloomBrightPass.glsl");
		m_BloomDownsampleShader = ShaderLoader::LoadShader("post_process/bloom/BloomDownsample.glsl");
		m_BloomUpsampleShader = ShaderLoader::LoadShader("post_process/bloom/BloomUpsample.glsl");
		m_BloomCompositeShader = ShaderLoader::LoadShader("post_process/bloom/BloomComposite.glsl");
		m_VignetteShader = ShaderLoader::LoadShader("post_process/vignette/vignette.glsl");
		m_ChromaticAberrationShader = ShaderLoader::LoadShader("post_process/chromatic_aberration/ChromaticAberration.glsl");
		m_FilmGrainShader = ShaderLoader::LoadShader("post_process/film_grain/FilmGrain.glsl");

		// Framebuffer setup
		m_SsaoRenderTarget.AddColorTexture(NormalizedSingleChannel8).CreateFramebuffer();
		m_SsaoBlurRenderTarget.AddColorTexture(NormalizedSingleChannel8).CreateFramebuffer();
		m_TonemappedNonLinearTarget.AddColorTexture(Normalized8).AddDepthStencilRBO(NormalizedDepthOnly).CreateFramebuffer();
		m_ResolveRenderTarget.AddColorTexture(FloatingPoint16).AddDepthStencilRBO(NormalizedDepthOnly).CreateFramebuffer();

		m_FullRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		m_HalfRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		m_QuarterRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		m_EighthRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();

		m_BrightPassRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		//m_BloomFullRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		m_BloomHalfRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		m_BloomQuarterRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		m_BloomEightRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		m_BloomSixteenRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		m_BloomThirtyTwoRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();
		m_BloomSixtyFourRenderTarget.AddColorTexture(FloatingPoint16).CreateFramebuffer();

		// SSAO Hemisphere Sample Generation (tangent space)
		std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
		std::default_random_engine generator;
		for (unsigned int i = 0; i < m_SsaoKernel.size(); i++)
		{
			// Make sure that the samples aren't perfectly perpendicular to the normal, or depth reconstruction will yield artifacts (so make sure the z value isn't close to 0)
			glm::vec3 hemisphereSample = glm::vec3((randomFloats(generator) * 2.0f) - 1.0f, (randomFloats(generator) * 2.0f) - 1.0f, glm::clamp((double)randomFloats(generator), 0.2, 1.0)); // Z = [0.2, 1] because we want hemisphere in tangent space
			hemisphereSample = glm::normalize(hemisphereSample);

			// Generate more samples closer to the origin of the hemisphere. Since these make for better light occlusion tests
			float scale = (float)i / m_SsaoKernel.size();
			scale = Lerp(0.1f, 1.0f, scale * scale);
			hemisphereSample *= scale;

			m_SsaoKernel[i] = hemisphereSample;
		}

		// SSAO Random Rotation Texture (used to apply a random rotation when constructing the change of basis matrix)
		// Random vectors should be in tangent space
		std::array<glm::vec3, 16> noiseSSAO;
		for (unsigned int i = 0; i < noiseSSAO.size(); i++)
		{
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
	PreLightingPassOutput PostProcessPass::ExecutePreLightingPass(GBuffer *inputGbuffer, ICamera *camera)
	{
		PreLightingPassOutput passOutput;
		if (!m_SsaoEnabled)
		{
			passOutput.ssaoTexture = AssetManager::GetInstance().GetWhiteTexture();
			return passOutput;
		}

		// Generate the AO factors for the scene
		glViewport(0, 0, m_SsaoRenderTarget.GetWidth(), m_SsaoRenderTarget.GetHeight());
		m_SsaoRenderTarget.Bind();
		m_GLCache->SetDepthTest(false);
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetCullFace(GL_BACK);

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

		inputGbuffer->GetNormal()->Bind(0);
		m_SsaoShader->SetUniform("normalTexture", 0);
		inputGbuffer->GetDepthStencilTexture()->Bind(1);
		m_SsaoShader->SetUniform("depthTexture", 1);
		m_SsaoNoiseTexture.Bind(2);
		m_SsaoShader->SetUniform("texNoise", 2);

		// Render our NDC quad to perform SSAO
		Renderer::DrawNdcPlane();

		// Blur the result
		m_SsaoBlurRenderTarget.Bind();
		m_SsaoBlurShader->Enable();

		m_SsaoBlurShader->SetUniform("numSamplesAroundTexel", 2); // 5x5 kernel blur
		m_SsaoBlurShader->SetUniform("ssaoInput", 0); // Texture unit
		m_SsaoRenderTarget.GetColourTexture()->Bind(0);

		// Render our NDC quad to blur our SSAO texture
		Renderer::DrawNdcPlane();
		
		// Reset unusual state
		m_GLCache->SetDepthTest(true);

		// Render pass output
		passOutput.ssaoTexture = m_SsaoBlurRenderTarget.GetColourTexture();
		return passOutput;
	}

	PostProcessPassOutput PostProcessPass::ExecutePostProcessPass(Framebuffer *framebufferToProcess)
	{
		PostProcessPassOutput output;

		GLCache *glCache = GLCache::GetInstance();

		// If the framebuffer is multi-sampled, resolve it
		Framebuffer *inputFramebuffer = framebufferToProcess;
		if (framebufferToProcess->IsMultisampled())
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferToProcess->GetFramebuffer());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ResolveRenderTarget.GetFramebuffer());
			glBlitFramebuffer(0, 0, framebufferToProcess->GetWidth(), framebufferToProcess->GetHeight(), 0, 0, m_ResolveRenderTarget.GetWidth(), m_ResolveRenderTarget.GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
			inputFramebuffer = &m_ResolveRenderTarget;
		}

		// Wireframe code otherwise we will just render a quad in wireframe
		if (Application::GetInstance().GetWireframe())
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Apply bloom if enabled, then Convert our scene from HDR (linear) -> SDR (sRGB) regardless if we apply bloom or not
		if (m_BloomEnabled)
		{
			Texture *sceneWithBloom = Bloom(inputFramebuffer->GetColourTexture());
			TonemapGammaCorrect(&m_TonemappedNonLinearTarget, sceneWithBloom);
		}
		else
		{
			TonemapGammaCorrect(&m_TonemappedNonLinearTarget, inputFramebuffer->GetColourTexture());
		}
		
		inputFramebuffer = &m_TonemappedNonLinearTarget;

		// Now apply various post processing effects after we are in SDR
		Framebuffer *framebufferToRenderTo = nullptr;
		if (m_ChromaticAberrationEnabled)
		{
			if (framebufferToRenderTo == &m_FullRenderTarget) framebufferToRenderTo = &m_TonemappedNonLinearTarget;
			else framebufferToRenderTo = &m_FullRenderTarget;

			ChromaticAberration(framebufferToRenderTo, inputFramebuffer->GetColourTexture());
			inputFramebuffer = framebufferToRenderTo;
		}

		if (m_FilmGrainEnabled)
		{
			if (framebufferToRenderTo == &m_FullRenderTarget) framebufferToRenderTo = &m_TonemappedNonLinearTarget;
			else framebufferToRenderTo = &m_FullRenderTarget;

			FilmGrain(framebufferToRenderTo, inputFramebuffer->GetColourTexture());
			inputFramebuffer = framebufferToRenderTo;
		}

		if (m_VignetteEnabled)
		{
			if (framebufferToRenderTo == &m_FullRenderTarget) framebufferToRenderTo = &m_TonemappedNonLinearTarget;
			else framebufferToRenderTo = &m_FullRenderTarget;

			Vignette(framebufferToRenderTo, inputFramebuffer->GetColourTexture());
			inputFramebuffer = framebufferToRenderTo;
		}

		if (m_FxaaEnabled)
		{
			if (framebufferToRenderTo == &m_FullRenderTarget) framebufferToRenderTo = &m_TonemappedNonLinearTarget;
			else framebufferToRenderTo = &m_FullRenderTarget;

			Fxaa(framebufferToRenderTo, inputFramebuffer->GetColourTexture());
			inputFramebuffer = framebufferToRenderTo;
		}

		// Finally return the output frame after being post processed
		output.outFramebuffer = inputFramebuffer;
		return output;
	}

	void PostProcessPass::TonemapGammaCorrect(Framebuffer *target, Texture *hdrTexture)
	{
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

		Renderer::DrawNdcPlane();
	}

	void PostProcessPass::Fxaa(Framebuffer *target, Texture *texture)
	{
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

		Renderer::DrawNdcPlane();
	}

	void PostProcessPass::Vignette(Framebuffer *target, Texture *texture, Texture *optionalVignetteMask)
	{
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
		if (optionalVignetteMask != nullptr)
		{
			m_VignetteShader->SetUniform("usesMask", 1);
			m_VignetteShader->SetUniform("vignette_mask", 1);
			optionalVignetteMask->Bind(1);
		}

		Renderer::DrawNdcPlane();
	}

	void PostProcessPass::ChromaticAberration(Framebuffer *target, Texture *texture)
	{
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

		Renderer::DrawNdcPlane();
	}

	void PostProcessPass::FilmGrain(Framebuffer *target, Texture *texture)
	{
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

		Renderer::DrawNdcPlane();
	}

	// https://www.youtube.com/watch?v=ml-5OGZC7vE
	// Great summary of the advanced warfare bloom talk and what Arcane's implementation is based on
	Texture* PostProcessPass::Bloom(Texture *hdrSceneTexture)
	{
		m_GLCache->SetDepthTest(false);
		m_GLCache->SetBlend(false);
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetCullFace(GL_BACK);
		m_GLCache->SetStencilTest(false);

		// Bloom Bright Pass
		glViewport(0, 0, m_BrightPassRenderTarget.GetWidth(), m_BrightPassRenderTarget.GetHeight());
		m_BrightPassRenderTarget.Bind();
		m_BrightPassRenderTarget.ClearAll();
		m_GLCache->SetShader(m_BloomBrightPassShader);
		m_BloomBrightPassShader->SetUniform("threshold", m_BloomThreshold);
		m_BloomBrightPassShader->SetUniform("sceneCapture", 0);
		hdrSceneTexture->Bind(0);
		Renderer::DrawNdcPlane();

		// Downsampling the parts of the scene that are above the luminance threshold using a 13 tap bilinear filter (Kawase downsample style)
		m_GLCache->SetShader(m_BloomDownsampleShader);
		glViewport(0, 0, m_BloomHalfRenderTarget.GetWidth(), m_BloomHalfRenderTarget.GetHeight());
		m_BloomHalfRenderTarget.Bind();
		m_BloomHalfRenderTarget.ClearAll();
		m_BloomDownsampleShader->SetUniform("texelSize", glm::vec2(1.0f / m_BloomHalfRenderTarget.GetWidth(), 1.0f / m_BloomHalfRenderTarget.GetHeight()));
		m_BloomDownsampleShader->SetUniform("textureToDownsample", 0);
		m_BrightPassRenderTarget.GetColourTexture()->Bind(0);
		Renderer::DrawNdcPlane();

		glViewport(0, 0, m_BloomQuarterRenderTarget.GetWidth(), m_BloomQuarterRenderTarget.GetHeight());
		m_BloomQuarterRenderTarget.Bind();
		m_BloomQuarterRenderTarget.ClearAll();
		m_BloomDownsampleShader->SetUniform("texelSize", glm::vec2(1.0f / m_BloomQuarterRenderTarget.GetWidth(), 1.0f / m_BloomQuarterRenderTarget.GetHeight()));
		m_BloomDownsampleShader->SetUniform("textureToDownsample", 0);
		m_BloomHalfRenderTarget.GetColourTexture()->Bind(0);
		Renderer::DrawNdcPlane();

		glViewport(0, 0, m_BloomEightRenderTarget.GetWidth(), m_BloomEightRenderTarget.GetHeight());
		m_BloomEightRenderTarget.Bind();
		m_BloomEightRenderTarget.ClearAll();
		m_BloomDownsampleShader->SetUniform("texelSize", glm::vec2(1.0f / m_BloomEightRenderTarget.GetWidth(), 1.0f / m_BloomEightRenderTarget.GetHeight()));
		m_BloomDownsampleShader->SetUniform("textureToDownsample", 0);
		m_BloomQuarterRenderTarget.GetColourTexture()->Bind(0);
		Renderer::DrawNdcPlane();

		glViewport(0, 0, m_BloomSixteenRenderTarget.GetWidth(), m_BloomSixteenRenderTarget.GetHeight());
		m_BloomSixteenRenderTarget.Bind();
		m_BloomSixteenRenderTarget.ClearAll();
		m_BloomDownsampleShader->SetUniform("texelSize", glm::vec2(1.0f / m_BloomSixteenRenderTarget.GetWidth(), 1.0f / m_BloomSixteenRenderTarget.GetHeight()));
		m_BloomDownsampleShader->SetUniform("textureToDownsample", 0);
		m_BloomEightRenderTarget.GetColourTexture()->Bind(0);
		Renderer::DrawNdcPlane();

		glViewport(0, 0, m_BloomThirtyTwoRenderTarget.GetWidth(), m_BloomThirtyTwoRenderTarget.GetHeight());
		m_BloomThirtyTwoRenderTarget.Bind();
		m_BloomThirtyTwoRenderTarget.ClearAll();
		m_BloomDownsampleShader->SetUniform("texelSize", glm::vec2(1.0f / m_BloomThirtyTwoRenderTarget.GetWidth(), 1.0f / m_BloomThirtyTwoRenderTarget.GetHeight()));
		m_BloomDownsampleShader->SetUniform("textureToDownsample", 0);
		m_BloomSixteenRenderTarget.GetColourTexture()->Bind(0);
		Renderer::DrawNdcPlane();

		glViewport(0, 0, m_BloomSixtyFourRenderTarget.GetWidth(), m_BloomSixtyFourRenderTarget.GetHeight());
		m_BloomSixtyFourRenderTarget.Bind();
		m_BloomSixtyFourRenderTarget.ClearAll();
		m_BloomDownsampleShader->SetUniform("texelSize", glm::vec2(1.0f / m_BloomSixtyFourRenderTarget.GetWidth(), 1.0f / m_BloomSixtyFourRenderTarget.GetHeight()));
		m_BloomDownsampleShader->SetUniform("textureToDownsample", 0);
		m_BloomThirtyTwoRenderTarget.GetColourTexture()->Bind(0);
		Renderer::DrawNdcPlane();

		// Upsampling using a 9 tap tent bilinear filter to get back to high res
		m_GLCache->SetShader(m_BloomUpsampleShader);
		m_GLCache->SetBlend(true);
		m_GLCache->SetBlendFunc(GL_ONE, GL_ONE);
		glViewport(0, 0, m_BloomThirtyTwoRenderTarget.GetWidth(), m_BloomThirtyTwoRenderTarget.GetHeight());
		m_BloomThirtyTwoRenderTarget.Bind();
		m_BloomUpsampleShader->SetUniform("sampleScale", glm::vec4(1.0, 1.0, 1.0, 1.0));
		m_BloomUpsampleShader->SetUniform("texelSize", glm::vec2(1.0f / m_BloomThirtyTwoRenderTarget.GetWidth(), 1.0f / m_BloomThirtyTwoRenderTarget.GetHeight()));
		m_BloomUpsampleShader->SetUniform("textureToUpsample", 0);
		m_BloomSixtyFourRenderTarget.GetColourTexture()->Bind(0);
		Renderer::DrawNdcPlane();

		glViewport(0, 0, m_BloomSixteenRenderTarget.GetWidth(), m_BloomSixteenRenderTarget.GetHeight());
		m_BloomSixteenRenderTarget.Bind();
		m_BloomUpsampleShader->SetUniform("sampleScale", glm::vec4(1.0, 1.0, 1.0, 1.0));
		m_BloomUpsampleShader->SetUniform("texelSize", glm::vec2(1.0f / m_BloomSixteenRenderTarget.GetWidth(), 1.0f / m_BloomSixteenRenderTarget.GetHeight()));
		m_BloomUpsampleShader->SetUniform("textureToUpsample", 0);
		m_BloomThirtyTwoRenderTarget.GetColourTexture()->Bind();
		Renderer::DrawNdcPlane();

		glViewport(0, 0, m_BloomEightRenderTarget.GetWidth(), m_BloomEightRenderTarget.GetHeight());
		m_BloomEightRenderTarget.Bind();
		m_BloomUpsampleShader->SetUniform("sampleScale", glm::vec4(1.0, 1.0, 1.0, 1.0));
		m_BloomUpsampleShader->SetUniform("texelSize", glm::vec2(1.0f / m_BloomEightRenderTarget.GetWidth(), 1.0f / m_BloomEightRenderTarget.GetHeight()));
		m_BloomUpsampleShader->SetUniform("textureToUpsample", 0);
		m_BloomSixteenRenderTarget.GetColourTexture()->Bind();
		Renderer::DrawNdcPlane();

		glViewport(0, 0, m_BloomQuarterRenderTarget.GetWidth(), m_BloomQuarterRenderTarget.GetHeight());
		m_BloomQuarterRenderTarget.Bind();
		m_BloomUpsampleShader->SetUniform("sampleScale", glm::vec4(1.0, 1.0, 1.0, 1.0));
		m_BloomUpsampleShader->SetUniform("texelSize", glm::vec2(1.0f / m_BloomQuarterRenderTarget.GetWidth(), 1.0f / m_BloomQuarterRenderTarget.GetHeight()));
		m_BloomUpsampleShader->SetUniform("textureToUpsample", 0);
		m_BloomEightRenderTarget.GetColourTexture()->Bind();
		Renderer::DrawNdcPlane();

		glViewport(0, 0, m_BloomHalfRenderTarget.GetWidth(), m_BloomHalfRenderTarget.GetHeight());
		m_BloomHalfRenderTarget.Bind();
		m_BloomUpsampleShader->SetUniform("sampleScale", glm::vec4(1.0, 1.0, 1.0, 1.0));
		m_BloomUpsampleShader->SetUniform("texelSize", glm::vec2(1.0f / m_BloomHalfRenderTarget.GetWidth(), 1.0f / m_BloomHalfRenderTarget.GetHeight()));
		m_BloomUpsampleShader->SetUniform("textureToUpsample", 0);
		m_BloomQuarterRenderTarget.GetColourTexture()->Bind();
		Renderer::DrawNdcPlane();

		// Combine our bloom texture with the scene
		m_GLCache->SetBlend(false);
		m_GLCache->SetShader(m_BloomCompositeShader);
		glViewport(0, 0, m_FullRenderTarget.GetWidth(), m_FullRenderTarget.GetHeight());
		m_FullRenderTarget.Bind();
		m_BloomCompositeShader->SetUniform("bloomStrength", m_BloomStrength);
		m_BloomCompositeShader->SetUniform("sceneTexture", 0);
		m_BloomCompositeShader->SetUniform("bloomTexture", 1);
		hdrSceneTexture->Bind(0);
		m_BloomHalfRenderTarget.GetColourTexture()->Bind(1);
		Renderer::DrawNdcPlane();

		return m_FullRenderTarget.GetColourTexture();
	}
}
