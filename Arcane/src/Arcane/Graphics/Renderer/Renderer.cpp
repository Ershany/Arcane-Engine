#include "arcpch.h"
#include "Renderer.h"

#include <Arcane/Graphics/Mesh/Model.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Shader.h>

namespace Arcane
{
	Quad* Renderer::s_NdcPlane = nullptr;
	Cube* Renderer::s_NdcCube = nullptr;
	RendererData Renderer::s_RendererData = {};
	GLCache* Renderer::s_GLCache = nullptr;
	std::deque<MeshDrawCallInfo> Renderer::s_OpaqueMeshDrawCallQueue;
	std::deque<MeshDrawCallInfo> Renderer::s_TransparentMeshDrawCallQueue;

	void Renderer::Init()
	{
		// Setup RendererData
		s_RendererData.DrawCallCount = 0;
		float maxAnisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
		s_RendererData.MaxAnisotropy = maxAnisotropy;

		s_GLCache = GLCache::GetInstance();

		s_NdcPlane = new Quad();
		s_NdcCube = new Cube();
	}

	void Renderer::Shutdown()
	{

	}

	void Renderer::BeginFrame()
	{
		s_RendererData.DrawCallCount = 0;
	}

	void Renderer::EndFrame()
	{

	}

	void Renderer::QueueMesh(Model *model, glm::mat4 &transform, bool isTransparent)
	{
		if (isTransparent)
		{
			s_TransparentMeshDrawCallQueue.emplace_back(MeshDrawCallInfo{ model, transform });
		}
		else
		{
			s_OpaqueMeshDrawCallQueue.emplace_back(MeshDrawCallInfo{ model, transform });
		}
	}

	void Renderer::Flush(ICamera *camera, Shader *shader, RenderPassType renderPassType)
	{
		s_GLCache->SetShader(shader);

		// Render opaque objects
		SetupOpaqueRenderState();
		while (!s_OpaqueMeshDrawCallQueue.empty())
		{
			MeshDrawCallInfo &current = s_OpaqueMeshDrawCallQueue.front();

			SetupModelMatrix(shader, current, renderPassType);
			current.model->Draw(shader, renderPassType);
			s_RendererData.DrawCallCount++;

			s_OpaqueMeshDrawCallQueue.pop_front();
		}

		// Render transparent objects (sort from back to front, does not account for rotations or scaling)
		SetupTransparentRenderState();
		std::sort(s_TransparentMeshDrawCallQueue.begin(), s_TransparentMeshDrawCallQueue.end(),
			[camera](MeshDrawCallInfo &a, MeshDrawCallInfo &b) -> bool
			{
				return glm::length2(camera->GetPosition() - glm::vec3(a.transform[3])) > glm::length2(camera->GetPosition() - glm::vec3(b.transform[3])); // transform[3] - Gets the translation part of the matrix
			});
		while (!s_TransparentMeshDrawCallQueue.empty())
		{
			MeshDrawCallInfo &current = s_TransparentMeshDrawCallQueue.front();

			SetupModelMatrix(shader, current, renderPassType);
			current.model->Draw(shader, renderPassType);
			s_RendererData.DrawCallCount++;

			s_TransparentMeshDrawCallQueue.pop_front();
		}
	}

	void Renderer::DrawNdcPlane()
	{
		s_NdcPlane->Draw();
		s_RendererData.DrawCallCount++;
	}

	void Renderer::DrawNdcCube()
	{
		s_NdcCube->Draw();
		s_RendererData.DrawCallCount++;
	}

	RendererData& Renderer::GetRendererData()
	{
		return s_RendererData;
	}

	void Renderer::SetupModelMatrix(Shader *shader, MeshDrawCallInfo &drawCallInfo, RenderPassType pass)
	{
#ifdef RENDERER_PARENT_TRANSFORMATIONS
		if (HasParent())
		{
			// Only apply scale locally
			model = glm::translate(glm::mat4(1.0f), entity->GetParent()->GetPosition()) * glm::toMat4(entity->GetParent()->GetOrientation()) * translate * rotate * scale; // translate, rotate, scale, are for the local object
		}
#endif
		shader->SetUniform("model", drawCallInfo.transform);

		if (pass == MaterialRequired)
		{
			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(drawCallInfo.transform)));
			shader->SetUniform("normalMatrix", normalMatrix);
		}
	}

	void Renderer::SetupOpaqueRenderState()
	{
		s_GLCache->SetDepthTest(true);
		s_GLCache->SetBlend(false);
		s_GLCache->SetFaceCull(true);
		s_GLCache->SetCullFace(GL_BACK);
	}

	void Renderer::SetupTransparentRenderState()
	{
		s_GLCache->SetDepthTest(true);
		s_GLCache->SetBlend(true);
		s_GLCache->SetFaceCull(false);
		s_GLCache->SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}