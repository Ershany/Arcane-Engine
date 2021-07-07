#include "arcpch.h"
#include "WaterPass.h"

#include <Arcane/Scene/Scene3D.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Texture/Texture.h>
#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Util/Loaders/TextureLoader.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>
#include <Arcane/Graphics/Renderer/Renderpass/Forward/ForwardLightingPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/ShadowmapPass.h>
#include <Arcane/UI/WaterPane.h>
#include <Arcane/UI/RuntimePane.h>

namespace Arcane
{
	WaterPass::WaterPass(Scene3D * scene) : RenderPass(scene), m_WaterEnabled(true), m_SceneRefractionFramebuffer(WATER_REFRACTION_RESOLUTION_WIDTH, WATER_REFRACTION_RESOLUTION_HEIGHT, false),
		m_WaterPos(1095.0f, 83.0f, 730.0f), m_WaterScale(600.0f), m_EnableClearWater(false), m_EnableShine(true), m_WaterTiling(6.0), m_WaveMoveFactor(0.0f), m_WaveSpeed(0.05f), m_WaterAlbedo(0.1f, 0.9f, 0.9f),
		m_AlbedoPower(0.1f), m_WaveStrength(0.02f), m_ShineDamper(25.0f), m_WaterNormalSmoothing(1.0f), m_DepthdampeningEffect(0.1f), m_ReflectionBias(2.0f), m_RefractionBias(2.0f)
#ifdef WATER_REFLECTION_USE_MSAA
		, m_SceneReflectionFramebuffer(WATER_REFLECTION_RESOLUTION_WIDTH, WATER_REFLECTION_RESOLUTION_HEIGHT, true)
		, m_ResolveFramebuffer(WATER_REFLECTION_RESOLUTION_WIDTH, WATER_REFLECTION_RESOLUTION_HEIGHT, false)
#else
		, m_SceneReflectionFramebuffer(WATER_REFLECTION_RESOLUTION_WIDTH, WATER_REFLECTION_RESOLUTION_HEIGHT, false)
#endif
	{
		m_WaterShader = ShaderLoader::LoadShader("Water.glsl");

		m_SceneReflectionFramebuffer.AddColorTexture(FloatingPoint16).AddDepthStencilRBO(NormalizedDepthOnly).CreateFramebuffer();
		m_SceneRefractionFramebuffer.AddColorTexture(FloatingPoint16).AddDepthStencilTexture(NormalizedDepthOnly).CreateFramebuffer();
#ifdef WATER_REFLECTION_USE_MSAA
		m_ResolveFramebuffer.AddColorTexture(FloatingPoint16).AddDepthStencilRBO(NormalizedDepthOnly).CreateFramebuffer();
#endif

		m_WaveTexture = TextureLoader::Load2DTexture(std::string("res/water/dudv.png"));
		m_WaterNormalMap = TextureLoader::Load2DTexture(std::string("res/water/normals.png"));

		// Values that can be tweaked in the editor
		WaterPane::BindClearWater(&m_EnableClearWater);
		WaterPane::BindEnableShine(&m_EnableShine);
		WaterPane::BindWaterTiling(&m_WaterTiling);
		WaterPane::BindWaterAlbedo(&m_WaterAlbedo);
		WaterPane::BindAlbedoPower(&m_AlbedoPower);
		WaterPane::BindWaveSpeed(&m_WaveSpeed);
		WaterPane::BindWaveStrength(&m_WaveStrength);
		WaterPane::BindShineDamper(&m_ShineDamper);
		WaterPane::BindWaterNormalSmoothing(&m_WaterNormalSmoothing);
		WaterPane::BindDepthDampeningEffect(&m_DepthdampeningEffect);
		WaterPane::BindReflectionBias(&m_ReflectionBias);
		WaterPane::BindRefractionBias(&m_RefractionBias);

		m_Timer.Reset();
	}

	WaterPass::~WaterPass()
	{

	}

	WaterPassOutput WaterPass::executeWaterPass(ShadowmapPassOutput &shadowmapData, LightingPassOutput &postTransparency, ICamera *camera)
	{
#if DEBUG_PROFILING
		glFinish();
		m_ProfilingTimer.Reset();
#endif // DEBUG_PROFILING

		WaterPassOutput passOutput;
		if (!m_WaterEnabled)
		{
			passOutput.outputFramebuffer = postTransparency.outputFramebuffer;
			return passOutput;
		}

		ModelRenderer *modelRenderer = m_ActiveScene->GetModelRenderer();
		DynamicLightManager *lightManager = m_ActiveScene->GetDynamicLightManager();
		m_GLCache->SetUsesClipPlane(true);

		// Generate Reflection framebuffer and render to it
		Framebuffer *reflectionFramebuffer = &m_SceneReflectionFramebuffer;
		{
			m_GLCache->SetClipPlane(glm::vec4(0.0f, 1.0f, 0.0f, -m_WaterPos.y + m_ReflectionBias));
			float distance = 2 * (camera->GetPosition().y - m_WaterPos.y);
			camera->SetPosition(camera->GetPosition() - glm::vec3(0.0f, distance, 0.0f));
			camera->InvertPitch();

			ForwardLightingPass lightingPass(m_ActiveScene, &m_SceneReflectionFramebuffer);
			lightingPass.executeLightingPass(shadowmapData, camera, false, false);

#ifdef WATER_REFLECTION_USE_MSAA
			glBindFramebuffer(GL_READ_FRAMEBUFFER, m_SceneReflectionFramebuffer.GetFramebuffer());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ResolveFramebuffer.GetFramebuffer());
			glBlitFramebuffer(0, 0, m_SceneReflectionFramebuffer.GetWidth(), m_SceneReflectionFramebuffer.GetHeight(), 0, 0, m_ResolveFramebuffer.GetWidth(), m_ResolveFramebuffer.GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
			reflectionFramebuffer = &m_ResolveFramebuffer;
#endif
			camera->SetPosition(camera->GetPosition() + glm::vec3(0.0f, distance, 0.0f));
			camera->InvertPitch();
		}

		// Generate Refraction framebuffer and render to it
		{
			m_GLCache->SetClipPlane(glm::vec4(0.0f, -1.0f, 0.0f, m_WaterPos.y + m_RefractionBias));

			ForwardLightingPass lightingPass(m_ActiveScene, &m_SceneRefractionFramebuffer);
			lightingPass.executeLightingPass(shadowmapData, camera, false, false);
		}

		m_GLCache->SetUsesClipPlane(false);

		// Finally render the water geometry and shade it
		m_GLCache->SetShader(m_WaterShader);
		postTransparency.outputFramebuffer->Bind();
		glViewport(0, 0, postTransparency.outputFramebuffer->GetWidth(), postTransparency.outputFramebuffer->GetHeight());
		if (postTransparency.outputFramebuffer->IsMultisampled()) {
			m_GLCache->SetMultisample(true);
		}
		else {
			m_GLCache->SetMultisample(false);
		}
		m_GLCache->SetDepthTest(true);
		m_GLCache->SetBlend(false);
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetCullFace(GL_BACK);

		glm::mat4 model(1);
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), m_WaterPos);
		glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(m_WaterScale, m_WaterScale, m_WaterScale));
		model = translate * rotate * scale;
		m_WaveMoveFactor = static_cast<float>(m_Timer.Elapsed() * m_WaveSpeed);
		m_WaveMoveFactor = static_cast<float>(std::fmod((double)m_WaveMoveFactor, 1.0));
		
		lightManager->BindLightingUniforms(m_WaterShader);
		m_WaterShader->SetUniform("view", camera->GetViewMatrix());
		m_WaterShader->SetUniform("projection", camera->GetProjectionMatrix());
		m_WaterShader->SetUniform("viewInverse", glm::inverse(camera->GetViewMatrix()));
		m_WaterShader->SetUniform("projectionInverse", glm::inverse(camera->GetProjectionMatrix()));
		m_WaterShader->SetUniform("clearWater", m_EnableClearWater);
		m_WaterShader->SetUniform("shouldShine", m_EnableShine);
		m_WaterShader->SetUniform("viewPos", camera->GetPosition());
		m_WaterShader->SetUniform("waterAlbedo", m_WaterAlbedo);
		m_WaterShader->SetUniform("albedoPower", m_AlbedoPower);
		m_WaterShader->SetUniform("model", model);
		m_WaterShader->SetUniform("waveTiling", m_WaterTiling);
		m_WaterShader->SetUniform("waveMoveFactor", m_WaveMoveFactor);
		m_WaterShader->SetUniform("nearFarPlaneValues", glm::vec2(NEAR_PLANE, FAR_PLANE));
		m_WaterShader->SetUniform("waveStrength", m_WaveStrength);
		m_WaterShader->SetUniform("shineDamper", m_ShineDamper);
		m_WaterShader->SetUniform("waterNormalSmoothing", m_WaterNormalSmoothing);
		m_WaterShader->SetUniform("depthDampeningEffect", m_DepthdampeningEffect);
		m_WaterShader->SetUniform("reflectionTexture", 0);
		reflectionFramebuffer->GetColourTexture()->Bind(0);
		m_WaterShader->SetUniform("refractionTexture", 1);
		m_SceneRefractionFramebuffer.GetColourTexture()->Bind(1);
		m_WaterShader->SetUniform("dudvWaveTexture", 2);
		m_WaveTexture->Bind(2);
		m_WaterShader->SetUniform("normalMap", 3);
		m_WaterNormalMap->Bind(3);
		m_WaterShader->SetUniform("refractionDepthTexture", 4);
		m_SceneRefractionFramebuffer.GetDepthStencilTexture()->Bind(4);

		m_WaterPlane.Draw();

#if DEBUG_PROFILING
		glFinish();
		RuntimePane::SetWaterTimer((float)m_ProfilingTimer.Elapsed());
#endif // DEBUG_PROFILING

		passOutput.outputFramebuffer = postTransparency.outputFramebuffer;
		return passOutput;
	}
}
