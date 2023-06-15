#include "arcpch.h"
#include "EditorPass.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Util/Loaders/AssetManager.h>
#include <Arcane/Scene/Scene.h>

namespace Arcane
{
	EditorPass::EditorPass(Scene *scene) : RenderPass(scene)
	{
		m_ColourWriteShader = ShaderLoader::LoadShader("ColourWrite.glsl");
		m_OutlineShader = ShaderLoader::LoadShader("Outline.glsl");
		m_UnlitSpriteShader = ShaderLoader::LoadShader("2D/UnlitSprite.glsl");
		m_DirectionalLightTexture = AssetManager::GetInstance().Load2DTextureAsync("res/editor/directional_light.png");
		m_PointLightTexture = AssetManager::GetInstance().Load2DTextureAsync("res/editor/point_light.png");
		m_SpotLightTexture = AssetManager::GetInstance().Load2DTextureAsync("res/editor/spot_light.png");
	}

	EditorPass::~EditorPass()
	{

	}

	EditorPassOutput EditorPass::ExecuteEditorPass(Framebuffer *sceneFramebuffer, Framebuffer *extraFramebuffer1, Framebuffer *extraFramebuffer2, ICamera *camera)
	{
		EditorPassOutput output;
		output.outFramebuffer = sceneFramebuffer;

		// Entity highlighting
		if (m_FocusedEntity.IsValid() && m_FocusedEntity.HasComponent<MeshComponent>())
		{
			auto& meshComponent = m_FocusedEntity.GetComponent<MeshComponent>();
			auto& transformComponent = m_FocusedEntity.GetComponent<TransformComponent>();

			PoseAnimator *poseAnimator = nullptr;
			if (m_FocusedEntity.HasComponent<PoseAnimatorComponent>())
			{
				poseAnimator = &m_FocusedEntity.GetComponent<PoseAnimatorComponent>().PoseAnimator;
			}

			glViewport(0, 0, extraFramebuffer1->GetWidth(), extraFramebuffer1->GetHeight());
			extraFramebuffer1->Bind();
			extraFramebuffer1->ClearAll();

			// Setup initial state
			m_GLCache->SetDepthTest(false);
			m_GLCache->SetStencilTest(false);
			m_GLCache->SetBlend(false);
			m_GLCache->SetMultisample(false);

			// Add objects that need to be outlined to the renderer
			m_GLCache->SetShader(m_ColourWriteShader);
			m_ColourWriteShader->SetUniform("colour", glm::vec3(1.0, 1.0, 1.0));
			Renderer::QueueMesh(meshComponent.AssetModel, transformComponent.GetTransform(), poseAnimator, meshComponent.IsTransparent, meshComponent.ShouldBackfaceCull);
			Renderer::Flush(camera, RenderPassType::NoMaterialRequired, m_ColourWriteShader, nullptr); // TODO: Add skinned shader

			// Combine the objects that need to be highlighted with the scene to get the final output
			glViewport(0, 0, extraFramebuffer2->GetWidth(), extraFramebuffer2->GetHeight());
			extraFramebuffer2->Bind();
			extraFramebuffer2->ClearAll();

			m_GLCache->SetShader(m_OutlineShader);
			m_OutlineShader->SetUniform("outlineSize", m_OutlineSize);
			m_OutlineShader->SetUniform("outlineColour", m_OutlineColour);
			m_OutlineShader->SetUniform("sceneTexture", 0);
			sceneFramebuffer->GetColourTexture()->Bind(0);
			m_OutlineShader->SetUniform("highlightTexture", 1);
			extraFramebuffer1->GetColourTexture()->Bind(1);
			Renderer::DrawNdcPlane();

			// Reset state
			m_GLCache->SetDepthTest(true);

			output.outFramebuffer = extraFramebuffer2; // Update the output framebuffer
		}

		// Debug Light Drawing
		{
			glViewport(0, 0, output.outFramebuffer->GetWidth(), output.outFramebuffer->GetHeight());
			output.outFramebuffer->Bind();
			output.outFramebuffer->ClearDepth(); // Clear depth, not needed and might cause our quad not to render

			// Setup state
			m_GLCache->SetDepthTest(false);
			m_GLCache->SetStencilTest(false);
			m_GLCache->SetBlend(false);
			m_GLCache->SetMultisample(false);
			
			m_GLCache->SetShader(m_UnlitSpriteShader);
			m_UnlitSpriteShader->SetUniform("view", camera->GetViewMatrix());
			m_UnlitSpriteShader->SetUniform("projection", camera->GetProjectionMatrix());

			bool shouldRenderQuads = false;
			auto group = m_ActiveScene->m_Registry.group<LightComponent>(entt::get<TransformComponent>);
			for (auto entity : group)
			{
				auto&[transformComponent, lightComponent] = group.get<TransformComponent, LightComponent>(entity);

				Texture *lightSprite = nullptr;
				switch (lightComponent.Type)
				{
				case LightType::LightType_Directional:
					lightSprite = m_DirectionalLightTexture;
					break;
				case LightType::LightType_Point:
					lightSprite = m_PointLightTexture;
					break;
				case LightType::LightType_Spot:
					lightSprite = m_SpotLightTexture;
					break;
				}

				Renderer::QueueQuad(transformComponent.GetTransform(), lightSprite);
			}
			Renderer::Flush(camera, NoMaterialRequired, m_UnlitSpriteShader, nullptr); // TODO: Add skinned shader

			// Reset State
			m_GLCache->SetDepthTest(true);
		}


		return output;
	}
}
