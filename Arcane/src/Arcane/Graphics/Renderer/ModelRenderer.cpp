#include "arcpch.h"
#include "ModelRenderer.h"

#include <Arcane/Graphics/Shader.h>
#include <Arcane/Graphics/Camera/FPSCamera.h>
#include <Arcane/Graphics/Renderer/GLCache.h>

namespace Arcane
{
	ModelRenderer::ModelRenderer(FPSCamera *camera) :
		m_Camera(camera), NDC_Plane(), NDC_Cube()
	{
		// Configure and cache OpenGL state
		m_GLCache = GLCache::GetInstance();
		m_GLCache->SetDepthTest(true);
		m_GLCache->SetBlend(false);
		m_GLCache->SetFaceCull(true);
	}

	void ModelRenderer::SubmitOpaque(RenderableModel *renderable) {
		m_OpaqueRenderQueue.push_back(renderable);
	}

	void ModelRenderer::SubmitTransparent(RenderableModel *renderable) {
		m_TransparentRenderQueue.push_back(renderable);
	}

	void ModelRenderer::SetupOpaqueRenderState() {
		m_GLCache->SetDepthTest(true);
		m_GLCache->SetBlend(false);
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetCullFace(GL_BACK);
	}
	
	void ModelRenderer::SetupTransparentRenderState() {
		m_GLCache->SetDepthTest(true);
		m_GLCache->SetBlend(true);
		m_GLCache->SetFaceCull(false);
	}

	void ModelRenderer::FlushOpaque(Shader *shader, RenderPassType pass) {
		m_GLCache->SetShader(shader);

		// Render opaque objects
		while (!m_OpaqueRenderQueue.empty()) {
			RenderableModel *current = m_OpaqueRenderQueue.front();

			SetupModelMatrix(current, shader, pass);
			current->Draw(shader, pass);

			m_OpaqueRenderQueue.pop_front();
		}
	}

	void ModelRenderer::FlushTransparent(Shader *shader, RenderPassType pass) {
		m_GLCache->SetShader(shader);

		// Sort then render transparent objects (from back to front, does not account for rotations or scaling)
		std::sort(m_TransparentRenderQueue.begin(), m_TransparentRenderQueue.end(),
			[this](RenderableModel *a, RenderableModel *b) -> bool
		{
			return glm::length2(m_Camera->GetPosition() - a->GetPosition()) > glm::length2(m_Camera->GetPosition() - b->GetPosition());
		});
		while (!m_TransparentRenderQueue.empty()) {
			RenderableModel *current = m_TransparentRenderQueue.front();

			m_GLCache->SetBlend(true);
			m_GLCache->SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			SetupModelMatrix(current, shader, pass);
			current->Draw(shader, pass);

			m_TransparentRenderQueue.pop_front();
		}
	}

	// TODO: Currently only supports two levels for hierarchical transformations
	// Make it work with any number of levels
	void ModelRenderer::SetupModelMatrix(RenderableModel *renderable, Shader *shader, RenderPassType pass) {
		glm::mat4 model(1);
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), renderable->GetPosition());
		glm::mat4 rotate = glm::toMat4(renderable->GetOrientation());
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), renderable->GetScale());

		if (renderable->GetParent()) {
			// Only apply scale locally
			model = glm::translate(glm::mat4(1.0f), renderable->GetParent()->GetPosition()) * glm::toMat4(renderable->GetParent()->GetOrientation()) * translate * rotate * scale;
		}
		else {
			model = translate * rotate * scale;
		}

		shader->SetUniform("model", model);

		if (pass == MaterialRequired) {
			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
			shader->SetUniform("normalMatrix", normalMatrix);
		}
	}
}
