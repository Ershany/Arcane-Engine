#include "arcpch.h"
#include "DeferredGeometryPass.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Scene/Scene.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>
#include <Arcane/Platform/OpenGL/Framebuffer/GBuffer.h>

namespace Arcane
{
	DeferredGeometryPass::DeferredGeometryPass(Scene *scene) : RenderPass(scene), m_AllocatedGBuffer(true)
	{
		m_ModelShader = ShaderLoader::LoadShader("deferred/PBR_Model_GeometryPass.glsl");
		m_SkinnedModelShader = ShaderLoader::LoadShader("deferred/PBR_Skinned_Model_GeometryPass.glsl");
		m_TerrainShader = ShaderLoader::LoadShader("deferred/PBR_Terrain_GeometryPass.glsl");

		m_GBuffer = new GBuffer(Window::GetRenderResolutionWidth(), Window::GetRenderResolutionHeight());
	}

	DeferredGeometryPass::DeferredGeometryPass(Scene *scene, GBuffer *customGBuffer) : RenderPass(scene), m_AllocatedGBuffer(false), m_GBuffer(customGBuffer)
	{
		m_ModelShader = ShaderLoader::LoadShader("deferred/PBR_Model_GeometryPass.glsl");
		m_TerrainShader = ShaderLoader::LoadShader("deferred/PBR_Terrain_GeometryPass.glsl");
	}

	DeferredGeometryPass::~DeferredGeometryPass()
	{
		if (m_AllocatedGBuffer) {
			delete m_GBuffer;
		}
	}

	GeometryPassOutput DeferredGeometryPass::ExecuteGeometryPass(ICamera *camera, bool renderOnlyStatic)
	{
		glViewport(0, 0, m_GBuffer->GetWidth(), m_GBuffer->GetHeight());
		m_GBuffer->Bind();
		m_GBuffer->ClearAll();
		m_GLCache->SetBlend(false);
		m_GLCache->SetMultisample(false);

		// Setup initial stencil state
		m_GLCache->SetStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		m_GLCache->SetStencilWriteMask(0x00);
		m_GLCache->SetStencilTest(true);

		// Setup model renderer for opaque objects only
		if (renderOnlyStatic)
		{
			m_ActiveScene->AddModelsToRenderer(ModelFilterType::OpaqueStaticModels);
		}
		else
		{
			m_ActiveScene->AddModelsToRenderer(ModelFilterType::OpaqueModels);
		}

		// Render opaque objects (use stencil to denote models for the deferred lighting pass)
		m_GLCache->SetStencilWriteMask(0xFF);
		m_GLCache->SetStencilFunc(GL_ALWAYS, StencilValue::ModelStencilValue, 0xFF);
		ARC_PUSH_RENDER_TAG("Skinned Models");
		Renderer::FlushOpaqueSkinnedMeshes(camera, RenderPassType::MaterialRequired, m_SkinnedModelShader);
		ARC_POP_RENDER_TAG();
		ARC_PUSH_RENDER_TAG("Non-Skinned Models");
		Renderer::FlushOpaqueNonSkinnedMeshes(camera, RenderPassType::MaterialRequired, m_ModelShader);
		ARC_POP_RENDER_TAG();
		m_GLCache->SetStencilWriteMask(0x00);

		Terrain *terrain = m_ActiveScene->GetTerrain();
		if (terrain)
		{
			// Setup terrain information
			ARC_PUSH_RENDER_TAG("Terrain");
			m_GLCache->SetShader(m_TerrainShader);
			m_TerrainShader->SetUniform("view", camera->GetViewMatrix());
			m_TerrainShader->SetUniform("projection", camera->GetProjectionMatrix());

			// Render the terrain (use stencil to denote the terrain for the deferred lighting pass)
			m_GLCache->SetStencilWriteMask(0xFF);
			m_GLCache->SetStencilFunc(GL_ALWAYS, StencilValue::TerrainStencilValue, 0xFF);
			terrain->Draw(m_TerrainShader, MaterialRequired);
			m_GLCache->SetStencilWriteMask(0x00);
			ARC_POP_RENDER_TAG();
		}

		// Reset state
		m_GLCache->SetStencilTest(false);

		// Render pass output
		GeometryPassOutput passOutput;
		passOutput.outputGBuffer = m_GBuffer;
		return passOutput;
	}
}
