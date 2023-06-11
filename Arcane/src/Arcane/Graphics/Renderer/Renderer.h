#pragma once

#include <Arcane/Graphics/Camera/ICamera.h>
#include <Arcane/Graphics/Mesh/Common/Cube.h>
#include <Arcane/Graphics/Mesh/Common/Quad.h>
#include <Arcane/Graphics/Renderer/Renderpass/RenderPassType.h>

#include <deque>

namespace Arcane
{
	class GLCache;
	class Model;
	class Shader;

	struct RendererData
	{
		float MaxAnisotropy;

		// Texture *BRDFLut;

		int DrawCallCount;
	};

	// TODO: Should eventually have a render ID and we can order drawcalls to avoid changing GPU state (shaders etc)
	// Some sort of draw call bucketing system
	struct MeshDrawCallInfo
	{
		Model *model;
		glm::mat4 transform;
		bool cullBackface;
	};
	struct QuadDrawCallInfo
	{
		const Texture *texture;
		glm::mat4 transform;
	};

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();

		static void QueueMesh(Model *model, const glm::mat4 &transform, bool isTransparent, bool cullBackface);
		static void QueueQuad(const glm::vec3 &position, const glm::vec2 &size, const Texture *texture); // TODO: Should use batch rendering to efficiently render quads together
		static void QueueQuad(const glm::mat4 &transform, const Texture *texture); // TODO: Should use batch rendering to efficiently render quads together

		static void Flush(ICamera *camera, Shader *shader, RenderPassType renderPassType);

		static void DrawNdcPlane();
		static void DrawNdcCube();

		static RendererData& GetRendererData();
	private:
		static void SetupModelMatrix(Shader *shader, MeshDrawCallInfo &drawCallInfo, RenderPassType pass);
		static void SetupModelMatrix(Shader *shader, QuadDrawCallInfo &drawCallInfo);
		static void SetupOpaqueRenderState();
		static void SetupTransparentRenderState();
		static void SetupQuadRenderState();
	private:
		static Quad *s_NdcPlane;
		static Cube *s_NdcCube;

		static RendererData s_RendererData;
		static GLCache *s_GLCache;

		static std::deque<MeshDrawCallInfo> s_OpaqueMeshDrawCallQueue;
		static std::deque<MeshDrawCallInfo> s_TransparentMeshDrawCallQueue;
		static std::deque<QuadDrawCallInfo> s_QuadDrawCallQueue;
	};
}
