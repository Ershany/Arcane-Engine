#include "arcpch.h"
#include "EditorPass.h"

#include <Arcane/Graphics/Shader.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Util/Loaders/AssetManager.h>
#include <Arcane/Scene/Scene.h>
#include <Arcane/Graphics/Renderer/DebugDraw3D.h>
#include <Arcane/Graphics/Camera/ICamera.h>

namespace Arcane
{
	EditorPass::EditorPass(Scene *scene) : RenderPass(scene)
	{
		m_ColourWriteShader = ShaderLoader::LoadShader("ColourWrite.glsl");
		m_ColourWriteShaderSkinned = ShaderLoader::LoadShader("ColourWriteSkinned.glsl");
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

		// Entity highlighting (should be done first since it might use debug rendering to highlight objects if no mesh exists to highlight)
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

			// Add objects that need to be outlined to the renderer (make them opaque so no sorting is done while we are writing to our outline shader)
			Renderer::QueueMesh(meshComponent.AssetModel, transformComponent.GetTransform(), poseAnimator, false, meshComponent.ShouldBackfaceCull);

			// Finally render our meshes (skinned and non-skinned)
			{
				m_GLCache->SetShader(m_ColourWriteShaderSkinned);
				m_ColourWriteShaderSkinned->SetUniform("colour", glm::vec3(1.0, 1.0, 1.0));
				Renderer::FlushOpaqueSkinnedMeshes(camera, RenderPassType::NoMaterialRequired, m_ColourWriteShaderSkinned);
			}
			{
				m_GLCache->SetShader(m_ColourWriteShader);
				m_ColourWriteShader->SetUniform("colour", glm::vec3(1.0, 1.0, 1.0));
				Renderer::FlushOpaqueNonSkinnedMeshes(camera, RenderPassType::NoMaterialRequired, m_ColourWriteShader);
			}

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
		else if (m_FocusedEntity.IsValid() && m_FocusedEntity.HasComponent<TransformComponent>())
		{
			auto& transformComponent = m_FocusedEntity.GetComponent<TransformComponent>();

			DebugDraw3D::QueueBox(transformComponent.Translation, transformComponent.Scale, m_OutlineColour);
		}

		// DebugDraw3D
		{
			glViewport(0, 0, output.outFramebuffer->GetWidth(), output.outFramebuffer->GetHeight());
			output.outFramebuffer->Bind();

			// Setup state
			m_GLCache->SetDepthTest(false);
			m_GLCache->SetStencilTest(false);
			m_GLCache->SetBlend(false);
			m_GLCache->SetMultisample(false);

			DebugDraw3D::FlushBatch(camera);
		}

		// Debug Light Drawing (can clear depth so do this last)
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
			Renderer::FlushQuads(camera, m_UnlitSpriteShader);

			// Reset State
			m_GLCache->SetDepthTest(true);
		}


		return output;
	}
}
