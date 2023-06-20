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
	std::deque<MeshDrawCallInfo> Renderer::s_OpaqueSkinnedMeshDrawCallQueue;
	std::deque<MeshDrawCallInfo> Renderer::s_TransparentMeshDrawCallQueue;
	std::deque<MeshDrawCallInfo> Renderer::s_TransparentSkinnedMeshDrawCallQueue;
	std::deque<QuadDrawCallInfo> Renderer::s_QuadDrawCallQueue;

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

	void Renderer::QueueQuad(const glm::vec3 &position, const glm::vec2 &size, const Texture *texture)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		QueueQuad(transform, texture);
	}

	void Renderer::QueueQuad(const glm::mat4 &transform, const Texture *texture) // TODO: Should use batch rendering to efficiently render quads together
	{
		s_QuadDrawCallQueue.emplace_back(QuadDrawCallInfo{ texture, transform });
	}

	void Renderer::QueueMesh(Model *model, const glm::mat4 &transform, PoseAnimator *animator/*= nullptr*/, bool isTransparent/*= false*/, bool cullBackface/*= true*/)
	{
		if (isTransparent)
		{
			if (animator)
			{
				s_TransparentSkinnedMeshDrawCallQueue.emplace_back(MeshDrawCallInfo{ model, animator, transform, cullBackface });
			}
			else
			{
				s_TransparentMeshDrawCallQueue.emplace_back(MeshDrawCallInfo{ model, nullptr, transform, cullBackface });
			}
		}
		else
		{
			if (animator)
			{
				s_OpaqueSkinnedMeshDrawCallQueue.emplace_back(MeshDrawCallInfo{ model, animator, transform, cullBackface });
			}
			else
			{
				s_OpaqueMeshDrawCallQueue.emplace_back(MeshDrawCallInfo{ model, nullptr, transform, cullBackface });
			}
		}
	}

	void Renderer::FlushOpaqueSkinnedMeshes(ICamera *camera, RenderPassType renderPassType, Shader *skinnedShader)
	{
		if (!s_OpaqueSkinnedMeshDrawCallQueue.empty())
		{
			s_GLCache->SetShader(skinnedShader);
			BindModelCameraInfo(camera, skinnedShader);
			SetupOpaqueRenderState();

			while (!s_OpaqueSkinnedMeshDrawCallQueue.empty())
			{
				MeshDrawCallInfo &current = s_OpaqueSkinnedMeshDrawCallQueue.front();

				s_GLCache->SetFaceCull(current.cullBackface);
				SetupModelMatrix(skinnedShader, current, renderPassType);
				SetupBoneMatrices(skinnedShader, current);
				current.model->Draw(skinnedShader, renderPassType);
				s_RendererData.DrawCallCount++;

				s_OpaqueSkinnedMeshDrawCallQueue.pop_front();
			}
		}
	}

	void Renderer::FlushOpaqueNonSkinnedMeshes(ICamera *camera, RenderPassType renderPassType, Shader *shader)
	{
		if (!s_OpaqueMeshDrawCallQueue.empty())
		{
			s_GLCache->SetShader(shader);
			BindModelCameraInfo(camera, shader);
			SetupOpaqueRenderState();

			while (!s_OpaqueMeshDrawCallQueue.empty())
			{
				MeshDrawCallInfo &current = s_OpaqueMeshDrawCallQueue.front();

				s_GLCache->SetFaceCull(current.cullBackface);
				SetupModelMatrix(shader, current, renderPassType);
				current.model->Draw(shader, renderPassType);
				s_RendererData.DrawCallCount++;

				s_OpaqueMeshDrawCallQueue.pop_front();
			}
		}
	}

	void Renderer::FlushTransparentSkinnedMeshes(ICamera *camera, RenderPassType renderPassType, Shader *skinnedShader)
	{
		// Sort from back to front, does not account for rotations, scaling, or animation
		if (!s_TransparentSkinnedMeshDrawCallQueue.empty())
		{
			s_GLCache->SetShader(skinnedShader);
			BindModelCameraInfo(camera, skinnedShader);
			SetupTransparentRenderState();

			std::sort(s_TransparentSkinnedMeshDrawCallQueue.begin(), s_TransparentSkinnedMeshDrawCallQueue.end(),
				[camera](MeshDrawCallInfo &a, MeshDrawCallInfo &b) -> bool
				{
					return glm::length2(camera->GetPosition() - glm::vec3(a.transform[3])) > glm::length2(camera->GetPosition() - glm::vec3(b.transform[3])); // transform[3] - Gets the translation part of the matrix
				});
			while (!s_TransparentSkinnedMeshDrawCallQueue.empty())
			{
				MeshDrawCallInfo &current = s_TransparentSkinnedMeshDrawCallQueue.front();

				s_GLCache->SetFaceCull(current.cullBackface);
				SetupModelMatrix(skinnedShader, current, renderPassType);
				SetupBoneMatrices(skinnedShader, current);
				current.model->Draw(skinnedShader, renderPassType);
				s_RendererData.DrawCallCount++;

				s_TransparentSkinnedMeshDrawCallQueue.pop_front();
			}
		}
	}

	void Renderer::FlushTransparentNonSkinnedMeshes(ICamera *camera, RenderPassType renderPassType, Shader *shader)
	{
		// Sort from back to front, does not account for rotations or scaling
		if (!s_TransparentMeshDrawCallQueue.empty())
		{
			s_GLCache->SetShader(shader);
			BindModelCameraInfo(camera, shader);
			SetupTransparentRenderState();

			std::sort(s_TransparentMeshDrawCallQueue.begin(), s_TransparentMeshDrawCallQueue.end(),
				[camera](MeshDrawCallInfo &a, MeshDrawCallInfo &b) -> bool
				{
					return glm::length2(camera->GetPosition() - glm::vec3(a.transform[3])) > glm::length2(camera->GetPosition() - glm::vec3(b.transform[3])); // transform[3] - Gets the translation part of the matrix
				});
			while (!s_TransparentMeshDrawCallQueue.empty())
			{
				MeshDrawCallInfo &current = s_TransparentMeshDrawCallQueue.front();

				s_GLCache->SetFaceCull(current.cullBackface);
				SetupModelMatrix(shader, current, renderPassType);
				current.model->Draw(shader, renderPassType);
				s_RendererData.DrawCallCount++;

				s_TransparentMeshDrawCallQueue.pop_front();
			}
		}
	}

	void Renderer::FlushQuads(ICamera *camera, Shader *shader)
	{
		if (!s_QuadDrawCallQueue.empty())
		{
			s_GLCache->SetShader(shader);
			BindQuadCameraInfo(camera, shader);
			static Quad localQuad(false);
			SetupQuadRenderState();

			while (!s_QuadDrawCallQueue.empty())
			{
				QuadDrawCallInfo &current = s_QuadDrawCallQueue.front();

				current.texture->Bind(5);
				shader->SetUniform("sprite", 5);
				SetupModelMatrix(shader, current);
				localQuad.Draw();
				s_RendererData.DrawCallCount++;

				s_QuadDrawCallQueue.pop_front();
			}
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

	void Renderer::BindModelCameraInfo(ICamera *camera, Shader *shader)
	{
		shader->SetUniform("viewPos", camera->GetPosition());
		shader->SetUniform("view", camera->GetViewMatrix());
		shader->SetUniform("projection", camera->GetProjectionMatrix());
	}

	void Renderer::BindQuadCameraInfo(ICamera *camera, Shader *shader)
	{
		shader->SetUniform("view", camera->GetViewMatrix());
		shader->SetUniform("projection", camera->GetProjectionMatrix());
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

	void Renderer::SetupModelMatrix(Shader *shader, QuadDrawCallInfo &drawCallInfo)
	{
#ifdef RENDERER_PARENT_TRANSFORMATIONS
		if (HasParent())
		{
			// Only apply scale locally
			model = glm::translate(glm::mat4(1.0f), entity->GetParent()->GetPosition()) * glm::toMat4(entity->GetParent()->GetOrientation()) * translate * rotate * scale; // translate, rotate, scale, are for the local object
		}
#endif
		shader->SetUniform("model", drawCallInfo.transform);
	}

	void Renderer::SetupBoneMatrices(Shader *shader, MeshDrawCallInfo &drawCallInfo)
	{
		if (drawCallInfo.animator)
		{
			const std::vector<glm::mat4> &matrices = drawCallInfo.animator->GetFinalBoneMatrices();
			shader->SetUniformArray("bonesMatrices", static_cast<int>(matrices.size()), &matrices[0]);
		}
	}

	void Renderer::SetupOpaqueRenderState()
	{
		s_GLCache->SetDepthTest(true);
		s_GLCache->SetBlend(false);
		s_GLCache->SetCullFace(GL_BACK);
	}

	void Renderer::SetupTransparentRenderState()
	{
		s_GLCache->SetDepthTest(true);
		s_GLCache->SetBlend(true);
		s_GLCache->SetCullFace(GL_BACK);
		s_GLCache->SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void Renderer::SetupQuadRenderState()
	{
		s_GLCache->SetDepthTest(true);
		s_GLCache->SetBlend(false);
		s_GLCache->SetFaceCull(false);
	}
}
