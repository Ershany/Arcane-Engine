#include "arcpch.h"
#include "DeferredGeometryPass.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Scene/Scene.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>

namespace Arcane
{
	DeferredGeometryPass::DeferredGeometryPass(Scene *scene) : RenderPass(scene), m_AllocatedGBuffer(true) {
		m_ModelShader = ShaderLoader::LoadShader("deferred/PBR_Model_GeometryPass.glsl");
		m_TerrainShader = ShaderLoader::LoadShader("deferred/PBR_Terrain_GeometryPass.glsl");

		m_GBuffer = new GBuffer(Window::GetRenderResolutionWidth(), Window::GetRenderResolutionHeight());
	}

	DeferredGeometryPass::DeferredGeometryPass(Scene *scene, GBuffer *customGBuffer) : RenderPass(scene), m_AllocatedGBuffer(false), m_GBuffer(customGBuffer) {
		m_ModelShader = ShaderLoader::LoadShader("deferred/PBR_Model_GeometryPass.glsl");
		m_TerrainShader = ShaderLoader::LoadShader("deferred/PBR_Terrain_GeometryPass.glsl");
	}

	DeferredGeometryPass::~DeferredGeometryPass() {
		if (m_AllocatedGBuffer) {
			delete m_GBuffer;
		}
	}

	GeometryPassOutput DeferredGeometryPass::executeGeometryPass(ICamera *camera, bool renderOnlyStatic) {
		glViewport(0, 0, m_GBuffer->GetWidth(), m_GBuffer->GetHeight());
		m_GBuffer->Bind();
		m_GBuffer->Clear();
		m_GLCache->SetBlend(false);
		m_GLCache->SetMultisample(false);

		// Setup initial stencil state
		m_GLCache->SetStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		m_GLCache->SetStencilWriteMask(0x00);
		m_GLCache->SetStencilTest(true);

		// Setup
		Terrain *terrain = m_ActiveScene->GetTerrain();

		m_GLCache->SetShader(m_ModelShader);
		m_ModelShader->SetUniform("viewPos", camera->GetPosition());
		m_ModelShader->SetUniform("view", camera->GetViewMatrix());
		m_ModelShader->SetUniform("projection", camera->GetProjectionMatrix());

		// Setup model renderer for opaque objects only
		if (renderOnlyStatic) {
			m_ActiveScene->AddModelsToRenderer(ModelFilterType::OpaqueStaticModels);
		}
		else {
			m_ActiveScene->AddModelsToRenderer(ModelFilterType::OpaqueModels);
		}

		// Render opaque objects (use stencil to denote models for the deferred lighting pass)
		m_GLCache->SetStencilWriteMask(0xFF);
		m_GLCache->SetStencilFunc(GL_ALWAYS, DeferredStencilValue::ModelStencilValue, 0xFF);
		Renderer::Flush(camera, m_ModelShader, RenderPassType::MaterialRequired);
		m_GLCache->SetStencilWriteMask(0x00);

		// Setup terrain information
		m_GLCache->SetShader(m_TerrainShader);
		m_TerrainShader->SetUniform("view", camera->GetViewMatrix());
		m_TerrainShader->SetUniform("projection", camera->GetProjectionMatrix());

		// Render the terrain (use stencil to denote the terrain for the deferred lighting pass)
		m_GLCache->SetStencilWriteMask(0xFF);
		m_GLCache->SetStencilFunc(GL_ALWAYS, DeferredStencilValue::TerrainStencilValue, 0xFF);
		terrain->Draw(m_TerrainShader, MaterialRequired);
		m_GLCache->SetStencilWriteMask(0x00);

		// Reset state
		m_GLCache->SetStencilTest(false);

		// Render pass output
		GeometryPassOutput passOutput;
		passOutput.outputGBuffer = m_GBuffer;
		return passOutput;
	}
}
