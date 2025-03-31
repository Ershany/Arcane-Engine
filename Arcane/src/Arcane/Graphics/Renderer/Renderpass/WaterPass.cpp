#include "arcpch.h"
#include "WaterPass.h"

#include <Arcane/Scene/Scene.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Texture/Texture.h>
#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Util/Loaders/AssetManager.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>
#include <Arcane/Graphics/Renderer/Renderpass/Forward/ForwardLightingPass.h>
#include <Arcane/Graphics/Renderer/Renderpass/ShadowmapPass.h>
#include <Arcane/Scene/Components.h>

namespace Arcane
{
	WaterPass::WaterPass(Scene * scene) : RenderPass(scene), m_WaterEnabled(true)
	{
		m_WaterShader = ShaderLoader::LoadShader("Water.glsl");

		AssetManager &assetManager = AssetManager::GetInstance();

		m_EffectsTimer.Reset();
	}

	WaterPass::~WaterPass()
	{

	}

	WaterPassOutput WaterPass::ExecuteWaterPass(ShadowmapPassOutput &inputShadowmapData, Framebuffer *inputFramebuffer, ICamera *camera)
	{
		WaterPassOutput passOutput;
		if (!m_WaterEnabled)
		{
			passOutput.outputFramebuffer = inputFramebuffer;
			return passOutput;
		}

		LightManager *lightManager = m_ActiveScene->GetLightManager();
		WaterManager *waterManager = m_ActiveScene->GetWaterManager();
		const WaterComponent *closestWaterWithReflectionRefraction = waterManager->GetClosestWaterComponent();

		auto group = m_ActiveScene->m_Registry.view<TransformComponent, WaterComponent>();
		for (auto entity : group)
		{
			auto&[transformComponent, waterComponent] = group.get<TransformComponent, WaterComponent>(entity);

			// Setup state
			m_GLCache->SetUsesClipPlane(true);

			// Only the closest water can receive reflection and refraction from the environment with planar reflections/refractions
			bool canReflectRefract = false;
			Framebuffer *reflectionFramebuffer = nullptr;
			Framebuffer *refractionFramebuffer = nullptr;
			if (closestWaterWithReflectionRefraction == &waterComponent)
			{
				canReflectRefract = true;
			}

			if (canReflectRefract)
			{
				reflectionFramebuffer = waterManager->GetWaterReflectionFramebuffer();
				refractionFramebuffer = waterManager->GetWaterRefractionFramebuffer();

				// Generate Reflection framebuffer and render to it
				ARC_PUSH_RENDER_TAG("Reflection");
				if (closestWaterWithReflectionRefraction->ReflectionEnabled)
				{
					glm::vec2 reflectionNearFarPlane = waterManager->GetClosestWaterReflectionNearFarPlane();
					float prevNearPlane = camera->GetNearPlane();
					float prevFarPlane = camera->GetFarPlane();

					m_GLCache->SetClipPlane(glm::vec4(0.0f, 1.0f, 0.0f, -transformComponent.Translation.y + waterComponent.ReflectionPlaneBias));
					float distance = 2 * (camera->GetPosition().y - transformComponent.Translation.y);
					camera->SetPosition(camera->GetPosition() - glm::vec3(0.0f, distance, 0.0f));
					camera->SetNearFarPlane(reflectionNearFarPlane.x, reflectionNearFarPlane.y);
					camera->InvertPitch();

					ForwardLightingPass lightingPass(m_ActiveScene, reflectionFramebuffer);
					LightingPassOutput output = lightingPass.ExecuteOpaqueLightingPass(inputShadowmapData, camera, false, false);
					lightingPass.ExecuteTransparentLightingPass(inputShadowmapData, output.outputFramebuffer, camera, false, false);

					// Check if reflection uses MSAA and resolve it if so
					if (closestWaterWithReflectionRefraction->ReflectionMSAA)
					{
						Framebuffer *reflectionResolveFramebuffer = waterManager->GetWaterReflectionResolveFramebuffer();
						glBindFramebuffer(GL_READ_FRAMEBUFFER, reflectionFramebuffer->GetFramebuffer());
						glBindFramebuffer(GL_DRAW_FRAMEBUFFER, reflectionResolveFramebuffer->GetFramebuffer());
						glBlitFramebuffer(0, 0, reflectionFramebuffer->GetWidth(), reflectionFramebuffer->GetHeight(), 0, 0, reflectionResolveFramebuffer->GetWidth(), reflectionResolveFramebuffer->GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
						reflectionFramebuffer = reflectionResolveFramebuffer; // Update reflection framebuffer to the resolved with no MSAA
					}

					camera->SetPosition(camera->GetPosition() + glm::vec3(0.0f, distance, 0.0f));
					camera->SetNearFarPlane(prevNearPlane, prevFarPlane);
					camera->InvertPitch();
				}
				ARC_POP_RENDER_TAG();

				// Generate Refraction framebuffer and render to it
				ARC_PUSH_RENDER_TAG("Refraction");
				if (closestWaterWithReflectionRefraction->RefractionEnabled)
				{
					glm::vec2 refractionNearFarPlane = waterManager->GetClosestWaterRefractionNearFarPlane();
					float prevNearPlane = camera->GetNearPlane();
					float prevFarPlane = camera->GetFarPlane();

					m_GLCache->SetClipPlane(glm::vec4(0.0f, -1.0f, 0.0f, transformComponent.Translation.y + waterComponent.RefractionPlaneBias));
					camera->SetNearFarPlane(refractionNearFarPlane.x, refractionNearFarPlane.y);

					ForwardLightingPass lightingPass(m_ActiveScene, refractionFramebuffer);
					LightingPassOutput output = lightingPass.ExecuteOpaqueLightingPass(inputShadowmapData, camera, false, false);
					lightingPass.ExecuteTransparentLightingPass(inputShadowmapData, output.outputFramebuffer, camera, false, false);

					// Check if refraction uses MSAA and resolve it if so
					if (closestWaterWithReflectionRefraction->RefractionMSAA)
					{
						Framebuffer *refractionResolveFramebuffer = waterManager->GetWaterRefractionResolveFramebuffer();
						glBindFramebuffer(GL_READ_FRAMEBUFFER, refractionFramebuffer->GetFramebuffer());
						glBindFramebuffer(GL_DRAW_FRAMEBUFFER, refractionResolveFramebuffer->GetFramebuffer());
						glBlitFramebuffer(0, 0, refractionFramebuffer->GetWidth(), refractionFramebuffer->GetHeight(), 0, 0, refractionResolveFramebuffer->GetWidth(), refractionResolveFramebuffer->GetHeight(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
						refractionFramebuffer = refractionResolveFramebuffer; // Update refraction framebuffer to the resolved with no MSAA
					}

					camera->SetNearFarPlane(prevNearPlane, prevFarPlane);
				}
				ARC_POP_RENDER_TAG();
			}

			// Reset State
			m_GLCache->SetUsesClipPlane(false);

			// Finally render the water geometry and shade it
			ARC_PUSH_RENDER_TAG("Water");
			m_GLCache->SetShader(m_WaterShader);
			inputFramebuffer->Bind();
			glViewport(0, 0, inputFramebuffer->GetWidth(), inputFramebuffer->GetHeight());
			if (inputFramebuffer->IsMultisampled())
			{
				m_GLCache->SetMultisample(true);
			}
			else
			{
				m_GLCache->SetMultisample(false);
			}
			m_GLCache->SetDepthTest(true);
			m_GLCache->SetBlend(false);
			m_GLCache->SetFaceCull(true);
			m_GLCache->SetCullFace(GL_BACK);

			glm::mat4 model(1);
			glm::mat4 translate = glm::translate(glm::mat4(1.0f), transformComponent.Translation);
			glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), transformComponent.Scale);
			model = translate * rotate * scale;
			waterComponent.MoveTimer = static_cast<float>(m_EffectsTimer.Elapsed() * waterComponent.WaveSpeed);
			waterComponent.MoveTimer = static_cast<float>(std::fmod((double)waterComponent.MoveTimer, 1.0));

			lightManager->BindLightingUniforms(m_WaterShader);
			m_WaterShader->SetUniform("view", camera->GetViewMatrix());
			m_WaterShader->SetUniform("projection", camera->GetProjectionMatrix());
			m_WaterShader->SetUniform("viewInverse", glm::inverse(camera->GetViewMatrix()));
			m_WaterShader->SetUniform("projectionInverse", glm::inverse(camera->GetProjectionMatrix()));
			m_WaterShader->SetUniform("clearWater", waterComponent.ClearWater);
			m_WaterShader->SetUniform("shouldShine", waterComponent.EnableShine);
			m_WaterShader->SetUniform("viewPos", camera->GetPosition());
			m_WaterShader->SetUniform("waterAlbedo", waterComponent.WaterAlbedo);
			m_WaterShader->SetUniform("albedoPower", waterComponent.AlbedoPower);
			m_WaterShader->SetUniform("model", model);
			m_WaterShader->SetUniform("waveTiling", waterComponent.WaterTiling);
			m_WaterShader->SetUniform("waveMoveFactor", waterComponent.MoveTimer);
			m_WaterShader->SetUniform("waveStrength", waterComponent.WaveStrength);
			m_WaterShader->SetUniform("shineDamper", waterComponent.ShineDamper);
			m_WaterShader->SetUniform("waterNormalSmoothing", waterComponent.NormalSmoothing);
			m_WaterShader->SetUniform("depthDampeningEffect", waterComponent.DepthDampening);

			// Only setup reflections/refractions if it is the closest water component received by the WaterManager, and it has those options enabled
			bool reflection = canReflectRefract && waterComponent.ReflectionEnabled;
			bool refraction = canReflectRefract && waterComponent.RefractionEnabled;
			m_WaterShader->SetUniform("reflectionEnabled", reflection);
			if (reflection)
			{
				m_WaterShader->SetUniform("reflectionTexture", 0);
				reflectionFramebuffer->GetColourTexture()->Bind(0);
			}
			m_WaterShader->SetUniform("refractionEnabled", refraction);
			if (refraction)
			{
				m_WaterShader->SetUniform("refractionTexture", 1);
				refractionFramebuffer->GetColourTexture()->Bind(1);
				m_WaterShader->SetUniform("refractionDepthTexture", 4);
				refractionFramebuffer->GetDepthStencilTexture()->Bind(4);
			}

			m_WaterShader->SetUniform("dudvWaveTexture", 2);
			if (waterComponent.WaterDistortionTexture)
				waterComponent.WaterDistortionTexture->Bind(2);
			else
				AssetManager::GetInstance().GetDefaultWaterDistortionTexture()->Bind(2);

			m_WaterShader->SetUniform("normalMap", 3);
			if (waterComponent.WaterNormalMap)
				waterComponent.WaterNormalMap->Bind(3);
			else
				AssetManager::GetInstance().GetDefaultNormalTexture()->Bind(3);

			m_WaterPlane.Draw();
			ARC_POP_RENDER_TAG();
		}

		passOutput.outputFramebuffer = inputFramebuffer;
		return passOutput;
	}
}
