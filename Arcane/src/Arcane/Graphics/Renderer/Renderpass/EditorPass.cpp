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

	EditorPassOutput EditorPass::ExecuteEditorPass(Framebuffer *sceneFramebuffer, Framebuffer *extraFramebuffer, ICamera *camera)
	{
		glViewport(0, 0, extraFramebuffer->GetWidth(), extraFramebuffer->GetHeight());
		extraFramebuffer->Bind();
		extraFramebuffer->Clear();
		
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
		m_ActiveScene->AddModelsToRenderer(ModelFilterType::OpaqueStaticModels);
		Renderer::Flush(camera, m_ColourWriteShader, RenderPassType::NoMaterialRequired);






		
		m_GLCache->SetShader(m_OutlineShader);
		m_OutlineShader->SetUniform("outlineSize", m_OutlineSize);
		m_OutlineShader->SetUniform("outlineColour", m_OutlineColour);
		Renderer::DrawNdcPlane();



		// Reset state
		m_GLCache->SetDepthTest(true);

		EditorPassOutput output;
		output.outFramebuffer = sceneFramebuffer;
		return output;
	}
}
