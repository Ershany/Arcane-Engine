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
		m_StencilWriteShader = ShaderLoader::LoadShader("StencilWrite.glsl");
		m_OutlineShader = ShaderLoader::LoadShader("Outline.glsl");
	}

	EditorPass::~EditorPass()
	{

	}

	EditorPassOutput EditorPass::ExecuteEditorPass(Framebuffer *sceneFramebuffer, Framebuffer *extraFramebuffer, ICamera *camera)
	{
		glViewport(0, 0, sceneFramebuffer->GetWidth(), sceneFramebuffer->GetHeight());
		sceneFramebuffer->Bind();
		
		m_GLCache->SetDepthTest(false);
		m_GLCache->SetStencilTest(true);
		m_GLCache->SetBlend(false);
		m_GLCache->SetMultisample(false);

		// Setup initial state
		m_GLCache->SetStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		m_GLCache->SetColourMask(false, false, false, false); // Do not write to the colour channel

		// Add objects that need to be outlined to the renderer
		m_GLCache->SetShader(m_StencilWriteShader);
		m_StencilWriteShader->SetUniform("view", camera->GetViewMatrix());
		m_StencilWriteShader->SetUniform("projection", camera->GetProjectionMatrix());
		m_ActiveScene->AddModelsToRenderer(ModelFilterType::OpaqueStaticModels);

		// Denote the models that should be outlined by rendering them with our outline stencil value
		m_GLCache->SetStencilWriteMask(0xFF); // Write stencil values
		m_GLCache->SetStencilFunc(GL_ALWAYS, StencilValue::OutlineStencilValue, 0xFF);
		Renderer::Flush(camera, m_StencilWriteShader, RenderPassType::NoMaterialRequired);

		// Reset some state
		m_GLCache->SetColourMask(true, true, true, true); // Write to the colour channel again
		m_GLCache->SetStencilWriteMask(0x00); // Do no write stencil values

		// Now we can outline any objects that have an outline stencil value
#if 0
		m_GLCache->SetShader(m_OutlineShader);
		m_OutlineShader->SetUniform("outlineSize", m_OutlineSize);
		m_OutlineShader->SetUniform("outlineColour", m_OutlineColour);
		m_GLCache->SetStencilFunc(GL_EQUAL, StencilValue::OutlineStencilValue, 0xFF);
		Renderer::DrawNdcPlane();
#endif

		// Reset state
		m_GLCache->SetDepthTest(true);
		m_GLCache->SetStencilTest(false);

		EditorPassOutput output;
		output.outFramebuffer = sceneFramebuffer;
		return output;
	}
}
