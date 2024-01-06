#pragma once
#ifndef DEBUGDRAW3D_H
#define DEBUGDRAW3D_H

/*
	This class can be used to render lines and shapes. This should not be used for gameplay purposes, because it isn't performant and the lines aren't anti aliased properly
	They are just lines for debugging purposes, displaying debug information in the editor, or scripting purposes when testing raycasts and other things
*/

namespace Arcane
{
	class ICamera;
	class GLCache;
	class Shader;
	class VertexArray;
	class Buffer;

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec3 Colour;
	};

	class DebugDraw3D
	{
	public:
		static void Init();

		static void BeginBatch();
		static void FlushBatch(ICamera *camera);

		static void QueueLine(const glm::vec3 &lineStart, const glm::vec3 &lineEnd, const glm::vec3 &colour);
		static void QueueBox(const glm::vec3 &boxPos, const glm::vec3 &boxScale, const glm::vec3 &colour);
		static void QueueBox(const glm::mat4 &transform, const glm::vec3 &colour);

		static void SetLineThickness(float thickness);
	private:
		static GLCache *s_GLCache;
		
		static Shader *s_LineShader;
		static VertexArray *s_LineVertexArray;
		static Buffer *s_LineVertexBuffer;
		static uint32_t s_LineVertexCount;
		static LineVertex *s_LineVertexBufferBase;
		static LineVertex *s_LineVertexBufferPtr;

		static const uint32_t s_MaxLineVertices = 2000; // 1000 lines limit (can be jacked up or re-visited to resize properly)
		static float s_LineThickness;
	};
}

#endif
