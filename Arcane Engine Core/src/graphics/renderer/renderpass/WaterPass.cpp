#include "pch.h"
#include "WaterPass.h"

#include <graphics/Shader.h>
#include <utils/loaders/ShaderLoader.h>
#include <graphics/renderer/renderpass/forward/ForwardLightingPass.h>
#include <graphics/renderer/renderpass/ShadowmapPass.h>

namespace arcane
{
	WaterPass::WaterPass(Scene3D * scene) : RenderPass(scene), m_WaterEnabled(true), m_SceneShadowFramebuffer(WATER_REFLECTION_SHADOW_RESOLUTION, WATER_REFLECTION_SHADOW_RESOLUTION, false), 
		m_SceneReflectionFramebuffer(WATER_REFLECTION_RESOLUTION_WIDTH, WATER_REFLECTION_RESOLUTION_HEIGHT, false), m_SceneRefractionFramebuffer(WATER_REFRACTION_RESOLUTION_WIDTH, WATER_REFRACTION_RESOLUTION_HEIGHT, false), 
		m_WaterPos(1095.0f, 83.0f, 730.0f), m_WaterScale(600.0f), m_EnableClearWater(false), m_WaveMoveFactor(0.0f), m_WaveSpeed(0.05f), m_WaterAlbedo(0.1f, 0.9f, 0.9f), m_AlbedoPower(0.1f)
	{
		m_WaterShader = ShaderLoader::loadShader("src/shaders/Water.glsl");

		m_SceneShadowFramebuffer.addDepthStencilTexture(NormalizedDepthOnly).createFramebuffer();
		m_SceneReflectionFramebuffer.addColorTexture(FloatingPoint16).addDepthStencilRBO(NormalizedDepthOnly).createFramebuffer();
		m_SceneRefractionFramebuffer.addColorTexture(FloatingPoint16).addDepthStencilTexture(NormalizedDepthOnly).createFramebuffer();

		m_WaveTexture = TextureLoader::load2DTexture(std::string("res/water/dudv.png"));
		m_WaterNormalMap = TextureLoader::load2DTexture(std::string("res/water/normals.png"));

		m_Timer.reset();
	}

	WaterPass::~WaterPass()
	{

	}

	WaterPassOutput WaterPass::executeWaterPass(LightingPassOutput &postTransparency, ICamera *camera)
	{
		WaterPassOutput passOutput;
		if (!m_WaterEnabled)
		{
			passOutput.outputFramebuffer = postTransparency.outputFramebuffer;
			return passOutput;
		}

		ModelRenderer *modelRenderer = m_ActiveScene->getModelRenderer();
		DynamicLightManager *lightManager = m_ActiveScene->getDynamicLightManager();
		m_GLCache->setUsesClipPlane(true);

		// Generate Reflection framebuffer and render to it
		{
			m_GLCache->setClipPlane(glm::vec4(0.0f, 1.0f, 0.0f, -m_WaterPos.y));
			float distance = 2 * (camera->getPosition().y - m_WaterPos.y);
			camera->setPosition(camera->getPosition() - glm::vec3(0.0f, distance, 0.0f));
			camera->invertPitch();

			ShadowmapPass shadowPass(m_ActiveScene, &m_SceneShadowFramebuffer);
			ForwardLightingPass lightingPass(m_ActiveScene, &m_SceneReflectionFramebuffer);

			ShadowmapPassOutput shadowpassOutput = shadowPass.generateShadowmaps(camera, false);
			lightingPass.executeLightingPass(shadowpassOutput, camera, false, false);

			camera->setPosition(camera->getPosition() + glm::vec3(0.0f, distance, 0.0f));
			camera->invertPitch();
		}

		// Generate Refraction framebuffer and render to it
		float refractionBias = 2.0f; // Should scale with dampeningEffectStrength
		{
			m_GLCache->setClipPlane(glm::vec4(0.0f, -1.0f, 0.0f, m_WaterPos.y + refractionBias));

			ShadowmapPass shadowPass(m_ActiveScene, &m_SceneShadowFramebuffer);
			ForwardLightingPass lightingPass(m_ActiveScene, &m_SceneRefractionFramebuffer);

			ShadowmapPassOutput shadowpassOutput = shadowPass.generateShadowmaps(camera, false);
			lightingPass.executeLightingPass(shadowpassOutput, camera, false, false);
		}

		m_GLCache->setUsesClipPlane(false);

		// Finally render the water geometry and shade it
		m_GLCache->switchShader(m_WaterShader);
		postTransparency.outputFramebuffer->bind();
		glViewport(0, 0, postTransparency.outputFramebuffer->getWidth(), postTransparency.outputFramebuffer->getHeight());
		if (postTransparency.outputFramebuffer->isMultisampled()) {
			m_GLCache->setMultisample(true);
		}
		else {
			m_GLCache->setMultisample(false);
		}
		m_GLCache->setDepthTest(true);
		m_GLCache->setBlend(false);
		m_GLCache->setFaceCull(true);
		m_GLCache->setCullFace(GL_BACK);

		glm::mat4 model(1);
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), m_WaterPos);
		glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(m_WaterScale, m_WaterScale, m_WaterScale));
		model = translate * rotate * scale;
		m_WaveMoveFactor = static_cast<float>(m_Timer.elapsed() * m_WaveSpeed);
		m_WaveMoveFactor = static_cast<float>(std::fmod((double)m_WaveMoveFactor, 1.0));
		
		lightManager->bindLightingUniforms(m_WaterShader);
		m_WaterShader->setUniform("view", camera->getViewMatrix());
		m_WaterShader->setUniform("projection", camera->getProjectionMatrix());
		m_WaterShader->setUniform("viewInverse", glm::inverse(camera->getViewMatrix()));
		m_WaterShader->setUniform("projectionInverse", glm::inverse(camera->getProjectionMatrix()));
		m_WaterShader->setUniform("clearWater", m_EnableClearWater);
		m_WaterShader->setUniform("viewPos", camera->getPosition());
		m_WaterShader->setUniform("waterAlbedo", m_WaterAlbedo);
		m_WaterShader->setUniform("albedoPower", m_AlbedoPower);
		m_WaterShader->setUniform("model", model);
		m_WaterShader->setUniform("waveTiling", m_WaterScale * 0.01f);
		m_WaterShader->setUniform("waveMoveFactor", m_WaveMoveFactor);
		m_WaterShader->setUniform("nearFarPlaneValues", glm::vec2(NEAR_PLANE, FAR_PLANE));
		m_WaterShader->setUniform("reflectionTexture", 0);
		m_SceneReflectionFramebuffer.getColourTexture()->bind(0);
		m_WaterShader->setUniform("refractionTexture", 1);
		m_SceneRefractionFramebuffer.getColourTexture()->bind(1);
		m_WaterShader->setUniform("dudvWaveTexture", 2);
		m_WaveTexture->bind(2);
		m_WaterShader->setUniform("normalMap", 3);
		m_WaterNormalMap->bind(3);
		m_WaterShader->setUniform("refractionDepthTexture", 4);
		m_SceneRefractionFramebuffer.getDepthStencilTexture()->bind(4);

		m_WaterPlane.Draw();

		passOutput.outputFramebuffer = postTransparency.outputFramebuffer;
		return passOutput;
	}
}
