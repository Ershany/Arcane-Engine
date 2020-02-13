#include "pch.h"
#include "PostProcessPass.h"

#include <ui/RuntimePane.h>
#include <utils/loaders/ShaderLoader.h>

namespace arcane {

	PostProcessPass::PostProcessPass(Scene3D *scene) : RenderPass(scene), m_SsaoRenderTarget((unsigned int)(Window::getRenderResolutionWidth() * 0.5f), (unsigned int)(Window::getRenderResolutionHeight() * 0.5f), false), m_SsaoBlurRenderTarget((unsigned int)(Window::getRenderResolutionWidth() * 0.5f), (unsigned int)(Window::getRenderResolutionHeight() * 0.5f), false),
		m_TonemappedNonLinearTarget(Window::getWidth(), Window::getHeight(), false), m_ScreenRenderTarget(Window::getWidth(), Window::getHeight(), false), m_ResolveRenderTarget(Window::getRenderResolutionWidth(), Window::getRenderResolutionHeight(), false), m_BrightPassRenderTarget(Window::getWidth(), Window::getHeight(), false),
		m_BloomFullRenderTarget(Window::getWidth(), Window::getHeight(), false), m_BloomHalfRenderTarget((unsigned int)(Window::getWidth() * 0.5f), (unsigned int)(Window::getHeight() * 0.5f), false), m_BloomQuarterRenderTarget((unsigned int)(Window::getWidth() * 0.25f), (unsigned int)(Window::getHeight() * 0.25f), false), m_BloomEightRenderTarget((unsigned int)(Window::getWidth() * 0.125f), (unsigned int)(Window::getHeight() * 0.125f), false),
		m_FullRenderTarget(Window::getWidth(), Window::getHeight(), false), m_HalfRenderTarget((unsigned int)(Window::getWidth() * 0.5f), (unsigned int)(Window::getHeight() * 0.5f), false), m_QuarterRenderTarget((unsigned int)(Window::getWidth() * 0.25f), (unsigned int)(Window::getWidth() * 0.25f), false), m_EightRenderTarget((unsigned int)(Window::getWidth() * 0.125f), (unsigned int)(Window::getHeight() * 0.125f), false),
		m_SsaoNoiseTexture(), m_ProfilingTimer(), m_EffectsTimer()
	{
		// Shader setup
		m_PassthroughShader = ShaderLoader::loadShader("src/shaders/post_process/Copy.glsl");
		m_TonemapGammaCorrectShader = ShaderLoader::loadShader("src/shaders/TonemapGammaCorrect.glsl");
		m_FxaaShader = ShaderLoader::loadShader("src/shaders/post_process/fxaa/FXAA.glsl");
		m_SsaoShader = ShaderLoader::loadShader("src/shaders/post_process/ssao/SSAO.glsl");
		m_SsaoBlurShader = ShaderLoader::loadShader("src/shaders/post_process/ssao/SSAO_Blur.glsl");
		m_BloomBrightPassShader = ShaderLoader::loadShader("src/shaders/post_process/bloom/BloomBrightPass.glsl");
		m_BloomGaussianBlurShader = ShaderLoader::loadShader("src/shaders/post_process/bloom/BloomGaussianBlur.glsl");
		m_BloomComposite = ShaderLoader::loadShader("src/shaders/post_process/bloom/Composite.glsl");
		m_VignetteShader = ShaderLoader::loadShader("src/shaders/post_process/vignette/vignette.glsl");
		m_ChromaticAberrationShader = ShaderLoader::loadShader("src/shaders/post_process/chromatic_aberration/ChromaticAberration.glsl");
		m_FilmGrainShader = ShaderLoader::loadShader("src/shaders/post_process/film_grain/FilmGrain.glsl");

		// Framebuffer setup
		m_SsaoRenderTarget.addColorTexture(NormalizedSingleChannel8).createFramebuffer();
		m_SsaoBlurRenderTarget.addColorTexture(NormalizedSingleChannel8).createFramebuffer();
		m_TonemappedNonLinearTarget.addColorTexture(Normalized8).addDepthStencilRBO(NormalizedDepthOnly).createFramebuffer();
		m_ScreenRenderTarget.addColorTexture(FloatingPoint16).addDepthStencilRBO(NormalizedDepthOnly).createFramebuffer();
		m_ResolveRenderTarget.addColorTexture(FloatingPoint16).addDepthStencilRBO(NormalizedDepthOnly).createFramebuffer();

		m_FullRenderTarget.addColorTexture(FloatingPoint16).createFramebuffer();
		m_HalfRenderTarget.addColorTexture(FloatingPoint16).createFramebuffer();
		m_QuarterRenderTarget.addColorTexture(FloatingPoint16).createFramebuffer();
		m_EightRenderTarget.addColorTexture(FloatingPoint16).createFramebuffer();

		m_BrightPassRenderTarget.addColorTexture(FloatingPoint16).createFramebuffer();
		m_BloomFullRenderTarget.addColorTexture(FloatingPoint16).createFramebuffer();
		m_BloomHalfRenderTarget.addColorTexture(FloatingPoint16).createFramebuffer();
		m_BloomQuarterRenderTarget.addColorTexture(FloatingPoint16).createFramebuffer();
		m_BloomEightRenderTarget.addColorTexture(FloatingPoint16).createFramebuffer();

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
		m_SsaoNoiseTexture.setTextureSettings(ssaoNoiseTextureSettings);
		m_SsaoNoiseTexture.generate2DTexture(4, 4, GL_RGB, GL_FLOAT, &noiseSSAO[0]);

		// Debug stuff
		DebugPane::bindFxaaEnabled(&m_FxaaEnabled);
		DebugPane::bindGammaCorrectionValue(&m_GammaCorrection);
		DebugPane::bindExposureValue(&m_Exposure);
		DebugPane::bindBloomThresholdValue(&m_BloomThreshold);
		DebugPane::bindSsaoEnabled(&m_SsaoEnabled);
		DebugPane::bindSsaoSampleRadiusValue(&m_SsaoSampleRadius);
		DebugPane::bindSsaoStrengthValue(&m_SsaoStrength);
		DebugPane::bindVignetteEnabled(&m_VignetteEnabled);
		DebugPane::bindVignetteIntensityValue(&m_VignetteIntensity);
		DebugPane::bindChromaticAberrationEnabled(&m_ChromaticAberrationEnabled);
		DebugPane::bindChromaticAberrationIntensityValue(&m_ChromaticAberrationIntensity);
		DebugPane::bindFilmGrainEnabled(&m_FilmGrainEnabled);
		DebugPane::bindFilmGrainIntensityValue(&m_FilmGrainIntensity);
	}

	PostProcessPass::~PostProcessPass() {}

	// Generates the AO of the scene using SSAO and stores it in a single channel texture
	PreLightingPassOutput PostProcessPass::executePreLightingPass(GeometryPassOutput &geometryData, ICamera *camera) {
#if DEBUG_ENABLED
		glFinish();
		m_ProfilingTimer.reset();
#endif
		PreLightingPassOutput passOutput;
		if (!m_SsaoEnabled) {
			passOutput.ssaoTexture = TextureLoader::getWhiteTexture();
			return passOutput;
		}

		// Generate the AO factors for the scene
		glViewport(0, 0, m_SsaoRenderTarget.getWidth(), m_SsaoRenderTarget.getHeight());
		m_SsaoRenderTarget.bind();
		m_GLCache->setDepthTest(false);
		m_GLCache->setFaceCull(true);
		m_GLCache->setCullFace(GL_BACK);

		// Setup
		ModelRenderer *modelRenderer = m_ActiveScene->getModelRenderer();

		// Bind the required data to perform SSAO
		m_GLCache->switchShader(m_SsaoShader);

		// Used to tile the noise texture across the screen every 4 texels (because our noise texture is 4x4)
		m_SsaoShader->setUniform("noiseScale", glm::vec2(m_SsaoRenderTarget.getWidth() * 0.25f, m_SsaoRenderTarget.getHeight() * 0.25f));

		m_SsaoShader->setUniform("ssaoStrength", m_SsaoStrength);
		m_SsaoShader->setUniform("sampleRadius", m_SsaoSampleRadius);
		m_SsaoShader->setUniform("sampleRadius2", m_SsaoSampleRadius * m_SsaoSampleRadius);
		m_SsaoShader->setUniform("numKernelSamples", (int)m_SsaoKernel.size());
		m_SsaoShader->setUniformArray("samples", m_SsaoKernel.size(), &m_SsaoKernel[0]);

		m_SsaoShader->setUniform("view", camera->getViewMatrix());
		m_SsaoShader->setUniform("projection", camera->getProjectionMatrix());
		m_SsaoShader->setUniform("viewInverse", glm::inverse(camera->getViewMatrix()));
		m_SsaoShader->setUniform("projectionInverse", glm::inverse(camera->getProjectionMatrix()));

		geometryData.outputGBuffer->getNormal()->bind(0);
		m_SsaoShader->setUniform("normalTexture", 0);
		geometryData.outputGBuffer->getDepthStencilTexture()->bind(1);
		m_SsaoShader->setUniform("depthTexture", 1);
		m_SsaoNoiseTexture.bind(2);
		m_SsaoShader->setUniform("texNoise", 2);

		// Render our NDC quad to perform SSAO
		modelRenderer->NDC_Plane.Draw();


		// Blur the result
		m_SsaoBlurRenderTarget.bind();
		m_SsaoBlurShader->enable();

		m_SsaoBlurShader->setUniform("numSamplesAroundTexel", 2); // 5x5 kernel blur
		m_SsaoBlurShader->setUniform("ssaoInput", 0); // Texture unit
		m_SsaoRenderTarget.getColourTexture()->bind(0);

		// Render our NDC quad to blur our SSAO texture
		modelRenderer->NDC_Plane.Draw();
		
		// Reset unusual state
		m_GLCache->setDepthTest(true);

#if DEBUG_ENABLED
		glFinish();
		RuntimePane::setSsaoTimer((float)m_ProfilingTimer.elapsed());
#endif

		// Render pass output
		passOutput.ssaoTexture = m_SsaoBlurRenderTarget.getColourTexture();
		return passOutput;
	}

	void PostProcessPass::executePostProcessPass(Framebuffer *framebufferToProcess) {
		ModelRenderer *modelRenderer = m_ActiveScene->getModelRenderer();
		GLCache *glCache = GLCache::getInstance();

		// If the framebuffer is multi-sampled, resolve it
		Framebuffer *supersampledTarget = framebufferToProcess;
		if (framebufferToProcess->isMultisampled()) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferToProcess->getFramebuffer());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ResolveRenderTarget.getFramebuffer());
			glBlitFramebuffer(0, 0, framebufferToProcess->getWidth(), framebufferToProcess->getHeight(), 0, 0, m_ResolveRenderTarget.getWidth(), m_ResolveRenderTarget.getHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
			supersampledTarget = &m_ResolveRenderTarget;
		}

		// If some sort of super-sampling is set, we need to downsample (or upsample) our image to match the window's resolution
		Framebuffer *inputFramebuffer = supersampledTarget;
		if (inputFramebuffer->getWidth() != m_ScreenRenderTarget.getWidth() || inputFramebuffer->getHeight() != m_ScreenRenderTarget.getHeight()) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, supersampledTarget->getFramebuffer());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ScreenRenderTarget.getFramebuffer());
			glBlitFramebuffer(0, 0, supersampledTarget->getWidth(), supersampledTarget->getHeight(), 0, 0, m_ScreenRenderTarget.getWidth(), m_ScreenRenderTarget.getHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
			inputFramebuffer = &m_ScreenRenderTarget;
		}

#if DEBUG_ENABLED
		if (DebugPane::getWireframeMode())
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
		//Texture *sceneWithBloom = bloom(target->getColourTexture());
		//tonemapGammaCorrect(&m_TonemappedNonLinearTarget, sceneWithBloom);

		// Convert our scene from HDR (linear) -> SDR (sRGB)
		tonemapGammaCorrect(&m_TonemappedNonLinearTarget, inputFramebuffer->getColourTexture());
		inputFramebuffer = &m_TonemappedNonLinearTarget;

		Framebuffer *framebufferToRenderTo = nullptr;
		if (m_ChromaticAberrationEnabled) {
			if (framebufferToRenderTo == &m_FullRenderTarget) framebufferToRenderTo = &m_TonemappedNonLinearTarget;
			else framebufferToRenderTo = &m_FullRenderTarget;

			chromaticAberration(framebufferToRenderTo, inputFramebuffer->getColourTexture());
			inputFramebuffer = framebufferToRenderTo;
		}

		if (m_FilmGrainEnabled) {
			if (framebufferToRenderTo == &m_FullRenderTarget) framebufferToRenderTo = &m_TonemappedNonLinearTarget;
			else framebufferToRenderTo = &m_FullRenderTarget;

			filmGrain(framebufferToRenderTo, inputFramebuffer->getColourTexture());
			inputFramebuffer = framebufferToRenderTo;
		}

		if (m_VignetteEnabled) {
			if (framebufferToRenderTo == &m_FullRenderTarget) framebufferToRenderTo = &m_TonemappedNonLinearTarget;
			else framebufferToRenderTo = &m_FullRenderTarget;

			vignette(framebufferToRenderTo, inputFramebuffer->getColourTexture());
			inputFramebuffer = framebufferToRenderTo;
		}

#if DEBUG_ENABLED
		glFinish();
		m_ProfilingTimer.reset();
#endif
		if (m_FxaaEnabled) {
			if (framebufferToRenderTo == &m_FullRenderTarget) framebufferToRenderTo = &m_TonemappedNonLinearTarget;
			else framebufferToRenderTo = &m_FullRenderTarget;

			fxaa(framebufferToRenderTo, inputFramebuffer->getColourTexture());
			inputFramebuffer = framebufferToRenderTo;
		}
#if DEBUG_ENABLED
		glFinish();
		RuntimePane::setFxaaTimer((float)m_ProfilingTimer.elapsed());
#endif

		// Finally render the scene to the window's framebuffer
		Window::bind();
		Window::clear();
		m_GLCache->switchShader(m_PassthroughShader);
		m_PassthroughShader->setUniform("input_texture", 0);
		inputFramebuffer->getColourTexture()->bind(0);
		m_ActiveScene->getModelRenderer()->NDC_Plane.Draw();
	}

	void PostProcessPass::tonemapGammaCorrect(Framebuffer *target, Texture *hdrTexture) {
		glViewport(0, 0, target->getWidth(), target->getHeight());
		m_GLCache->switchShader(m_TonemapGammaCorrectShader);
		m_GLCache->setDepthTest(false);
		m_GLCache->setBlend(false);
		m_GLCache->setFaceCull(true);
		m_GLCache->setCullFace(GL_BACK);
		m_GLCache->setStencilTest(false);
		target->bind();

		m_TonemapGammaCorrectShader->setUniform("gamma_inverse", 1.0f / m_GammaCorrection); 
		m_TonemapGammaCorrectShader->setUniform("exposure", m_Exposure);
		m_TonemapGammaCorrectShader->setUniform("input_texture", 0);
		hdrTexture->bind(0);

		m_ActiveScene->getModelRenderer()->NDC_Plane.Draw();
	}

	void PostProcessPass::fxaa(Framebuffer *target, Texture *texture) {
		glViewport(0, 0, target->getWidth(), target->getHeight());
		m_GLCache->switchShader(m_FxaaShader);
		m_GLCache->setDepthTest(false);
		m_GLCache->setBlend(false);
		m_GLCache->setFaceCull(true);
		m_GLCache->setCullFace(GL_BACK);
		m_GLCache->setStencilTest(false);
		target->bind();

		m_FxaaShader->setUniform("texel_size", glm::vec2(1.0f / (float)texture->getWidth(), 1.0f / (float)texture->getHeight()));
		m_FxaaShader->setUniform("input_texture", 0);
		texture->bind(0);

		m_ActiveScene->getModelRenderer()->NDC_Plane.Draw();
	}

	void PostProcessPass::vignette(Framebuffer *target, Texture *texture, Texture *optionalVignetteMask) {
		glViewport(0, 0, target->getWidth(), target->getHeight());
		m_GLCache->switchShader(m_VignetteShader);
		m_GLCache->setDepthTest(false);
		m_GLCache->setBlend(false);
		m_GLCache->setFaceCull(true);
		m_GLCache->setCullFace(GL_BACK);
		m_GLCache->setStencilTest(false);
		target->bind();

		m_VignetteShader->setUniform("colour", m_VignetteColour);
		m_VignetteShader->setUniform("intensity", m_VignetteIntensity);
		m_VignetteShader->setUniform("input_texture", 0);
		texture->bind(0);
		if (optionalVignetteMask != nullptr) {
			m_VignetteShader->setUniform("usesMask", 1);
			m_VignetteShader->setUniform("vignette_mask", 1);
			optionalVignetteMask->bind(1);
		}

		m_ActiveScene->getModelRenderer()->NDC_Plane.Draw();
	}

	void PostProcessPass::chromaticAberration(Framebuffer *target, Texture *texture) {
		glViewport(0, 0, target->getWidth(), target->getHeight());
		m_GLCache->switchShader(m_ChromaticAberrationShader);
		m_GLCache->setDepthTest(false);
		m_GLCache->setBlend(false);
		m_GLCache->setFaceCull(true);
		m_GLCache->setCullFace(GL_BACK);
		m_GLCache->setStencilTest(false);
		target->bind();

		m_ChromaticAberrationShader->setUniform("intensity", m_ChromaticAberrationIntensity * 100);
		m_ChromaticAberrationShader->setUniform("texel_size", glm::vec2(1.0f / (float)texture->getWidth(), 1.0f / (float)texture->getHeight()));
		m_ChromaticAberrationShader->setUniform("input_texture", 0);
		texture->bind(0);

		m_ActiveScene->getModelRenderer()->NDC_Plane.Draw();
	}

	void PostProcessPass::filmGrain(Framebuffer *target, Texture *texture) {
		glViewport(0, 0, target->getWidth(), target->getHeight());
		m_GLCache->switchShader(m_FilmGrainShader);
		m_GLCache->setDepthTest(false);
		m_GLCache->setBlend(false);
		m_GLCache->setFaceCull(true);
		m_GLCache->setCullFace(GL_BACK);
		m_GLCache->setStencilTest(false);
		target->bind();

		m_FilmGrainShader->setUniform("intensity", m_FilmGrainIntensity * 100.0f);
		m_FilmGrainShader->setUniform("time", (float)(std::fmod(m_EffectsTimer.elapsed(), 100.0)));
		m_FilmGrainShader->setUniform("input_texture", 0);
		texture->bind(0);

		m_ActiveScene->getModelRenderer()->NDC_Plane.Draw();
	}

	Texture* PostProcessPass::bloom(Texture *hdrSceneTexture) {
		m_GLCache->setDepthTest(false);
		m_GLCache->setBlend(false);
		m_GLCache->setFaceCull(true);
		m_GLCache->setCullFace(GL_BACK);
		m_GLCache->setStencilTest(false);

		// Bloom Bright Pass
		glViewport(0, 0, m_BrightPassRenderTarget.getWidth(), m_BrightPassRenderTarget.getHeight());
		m_BrightPassRenderTarget.bind();
		m_BrightPassRenderTarget.clear();
		m_GLCache->switchShader(m_BloomBrightPassShader);
		m_BloomBrightPassShader->setUniform("threshold", m_BloomThreshold);
		m_BloomBrightPassShader->setUniform("scene_capture", 0);
		hdrSceneTexture->bind(0);
		m_ActiveScene->getModelRenderer()->NDC_Plane.Draw();

		// Bloom Gaussian Blur Pass
		// As the render target gets smaller, we can increase the separable (two-pass) Gaussian kernel size
		m_GLCache->switchShader(m_BloomGaussianBlurShader);
		glViewport(0, 0, m_FullRenderTarget.getWidth(), m_FullRenderTarget.getHeight());
		m_FullRenderTarget.bind();
		m_FullRenderTarget.clear();
		m_BloomGaussianBlurShader->setUniform("isVerticalBlur", true);
		m_BloomGaussianBlurShader->setUniform("read_offset", glm::vec2(1.0f / (float)m_FullRenderTarget.getWidth(), 1.0f / (float)m_FullRenderTarget.getHeight()));
		m_BloomGaussianBlurShader->setUniform("bloom_texture", 0);
		m_BrightPassRenderTarget.getColourTexture()->bind(0);
		m_ActiveScene->getModelRenderer()->NDC_Plane.Draw();

		m_BloomFullRenderTarget.bind();
		m_BloomFullRenderTarget.clear();
		m_BloomGaussianBlurShader->setUniform("isVerticalBlur", false);
		m_BloomGaussianBlurShader->setUniform("read_offset", glm::vec2(1.0f / (float)m_BloomFullRenderTarget.getWidth(), 1.0f / (float)m_BloomFullRenderTarget.getHeight()));
		m_BloomGaussianBlurShader->setUniform("bloom_texture", 0);
		m_FullRenderTarget.getColourTexture()->bind(0);
		m_ActiveScene->getModelRenderer()->NDC_Plane.Draw();

		// Combine our bloom texture with the scene
		m_GLCache->switchShader(m_BloomComposite);
		glViewport(0, 0, m_FullRenderTarget.getWidth(), m_FullRenderTarget.getHeight());
		m_FullRenderTarget.bind();
		m_BloomComposite->setUniform("strength", 1.0f);
		m_BloomComposite->setUniform("scene_texture", 0);
		m_BloomComposite->setUniform("bloom_texture", 1);
		hdrSceneTexture->bind(0);
		m_BloomFullRenderTarget.getColourTexture()->bind(1);
		m_ActiveScene->getModelRenderer()->NDC_Plane.Draw();

		return m_FullRenderTarget.getColourTexture();
	}

}
