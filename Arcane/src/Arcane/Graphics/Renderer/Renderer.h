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
	};

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();

		static void QueueMesh(Model *model, glm::mat4 &transform, bool isTransparent=false);

		static void Flush(ICamera *camera, Shader *shader, RenderPassType renderPassType);

		static void DrawNdcPlane();
		static void DrawNdcCube();

		static RendererData& GetRendererData();
	private:
		static void SetupModelMatrix(Shader *shader, MeshDrawCallInfo &drawCallInfo, RenderPassType pass);
		static void SetupOpaqueRenderState();
		static void SetupTransparentRenderState();
	private:
		static Quad *s_NdcPlane;
		static Cube *s_NdcCube;

		static RendererData s_RendererData;
		static GLCache *s_GLCache;

		static std::deque<MeshDrawCallInfo> s_OpaqueMeshDrawCallQueue;
		static std::deque<MeshDrawCallInfo> s_TransparentMeshDrawCallQueue;
	};
}
