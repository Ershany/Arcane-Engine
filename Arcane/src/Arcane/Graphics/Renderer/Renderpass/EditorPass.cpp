#include "arcpch.h"
#include "EditorPass.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Scene/Scene.h>

namespace Arcane
{
	EditorPass::EditorPass(Scene *scene) : RenderPass(scene)
	{
		m_ColourWriteShader = ShaderLoader::LoadShader("ColourWrite.glsl");
		m_OutlineShader = ShaderLoader::LoadShader("Outline.glsl");
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
			auto meshComponent = m_FocusedEntity.GetComponent<MeshComponent>();
			auto transformComponent = m_FocusedEntity.GetComponent<TransformComponent>();

			glViewport(0, 0, extraFramebuffer1->GetWidth(), extraFramebuffer1->GetHeight());
			extraFramebuffer1->Bind();
			extraFramebuffer1->Clear();

			// Setup initial state
			m_GLCache->SetDepthTest(false);
			m_GLCache->SetStencilTest(false);
			m_GLCache->SetBlend(false);
			m_GLCache->SetMultisample(false);

			// Add objects that need to be outlined to the renderer
			m_GLCache->SetShader(m_ColourWriteShader);
			m_ColourWriteShader->SetUniform("colour", glm::vec3(1.0, 1.0, 1.0));
			m_ColourWriteShader->SetUniform("view", camera->GetViewMatrix());
			m_ColourWriteShader->SetUniform("projection", camera->GetProjectionMatrix());
			Renderer::QueueMesh(meshComponent.AssetModel, transformComponent.GetTransform(), meshComponent.IsTransparent);
			Renderer::Flush(camera, m_ColourWriteShader, RenderPassType::NoMaterialRequired);

			// Combine the objects that need to be highlighted with the scene to get the final output
			glViewport(0, 0, extraFramebuffer2->GetWidth(), extraFramebuffer2->GetHeight());
			extraFramebuffer2->Bind();
			extraFramebuffer2->Clear();

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

		return output;
	}
}
