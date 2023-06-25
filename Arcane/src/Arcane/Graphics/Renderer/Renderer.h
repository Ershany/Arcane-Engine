#pragma once

#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Graphics/Mesh/Common/Cube.h>
#include <Arcane/Graphics/Mesh/Common/Quad.h>
#include <Arcane/Graphics/Renderer/Renderpass/RenderPassType.h>
#include <Arcane/Animation/PoseAnimator.h>

#include <deque>

namespace Arcane
{
	class GLCache;
	class Model;
	class Shader;

	struct RendererData
	{
		// Renderer Parameters
		float MaxAnisotropy;
		// Texture *BRDFLut;

		// Draw Call Statistics
		unsigned int DrawCallCount;
		unsigned int MeshesDrawnCount;
		unsigned int QuadsDrawnCount;
	};

	// TODO: Should eventually have a render ID and we can order drawcalls to avoid changing GPU state (shaders etc)
	// Some sort of draw call bucketing system
	struct MeshDrawCallInfo
	{
		Model *model = nullptr;
		PoseAnimator *animator = nullptr;
		glm::mat4 transform;
		bool cullBackface;
	};
	struct QuadDrawCallInfo
	{
		const Texture *texture = nullptr;
		glm::mat4 transform;
	};

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();

		static void QueueMesh(Model *model, const glm::mat4 &transform, PoseAnimator *animator = nullptr, bool isTransparent = false, bool cullBackface = true);
		static void QueueQuad(const glm::vec3 &position, const glm::vec2 &size, const Texture *texture); // TODO: Should use batch rendering to efficiently render quads together
		static void QueueQuad(const glm::mat4 &transform, const Texture *texture); // TODO: Should use batch rendering to efficiently render quads together

		static void FlushOpaqueSkinnedMeshes(ICamera *camera, RenderPassType renderPassType, Shader *skinnedShader);
		static void FlushOpaqueNonSkinnedMeshes(ICamera *camera, RenderPassType renderPassType, Shader *shader);
		static void FlushTransparentSkinnedMeshes(ICamera *camera, RenderPassType renderPassType, Shader *skinnedShader);
		static void FlushTransparentNonSkinnedMeshes(ICamera *camera, RenderPassType renderPassType, Shader *shader);
		static void FlushQuads(ICamera *camera, Shader *shader);

		static void DrawNdcPlane();
		static void DrawNdcCube();

		static const RendererData& GetRendererData();
	private:
		static void BindModelCameraInfo(ICamera *camera, Shader *shader);
		static void BindQuadCameraInfo(ICamera *camera, Shader *shader);
		static void SetupModelMatrix(Shader *shader, MeshDrawCallInfo &drawCallInfo, RenderPassType pass);
		static void SetupModelMatrix(Shader *shader, QuadDrawCallInfo &drawCallInfo);
		static void SetupBoneMatrices(Shader *shader, MeshDrawCallInfo &drawCallInfo);
		static void SetupOpaqueRenderState();
		static void SetupTransparentRenderState();
		static void SetupQuadRenderState();
	private:
		static Quad *s_NdcPlane;
		static Cube *s_NdcCube;

		static RendererData s_RendererData;
		static GLCache *s_GLCache;

		static std::deque<MeshDrawCallInfo> s_OpaqueMeshDrawCallQueue;
		static std::deque<MeshDrawCallInfo> s_OpaqueSkinnedMeshDrawCallQueue;
		static std::deque<MeshDrawCallInfo> s_TransparentMeshDrawCallQueue;
		static std::deque<MeshDrawCallInfo> s_TransparentSkinnedMeshDrawCallQueue;
		static std::deque<QuadDrawCallInfo> s_QuadDrawCallQueue;

		static unsigned int m_CurrentDrawCallCount;
		static unsigned int m_CurrentMeshesDrawnCount;
		static unsigned int m_CurrentQuadsDrawnCount;
	};
}
