#include "arcpch.h"
#include "DebugDraw3D.h"

#include <Arcane/Graphics/Shader.h>
#include <Arcane/Platform/OpenGL/VertexArray.h>
#include <Arcane/Platform/OpenGL/Buffer.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Camera/ICamera.h>

namespace Arcane
{
	GLCache *DebugDraw3D::s_GLCache = nullptr;
	Shader *DebugDraw3D::s_LineShader = nullptr;
	VertexArray *DebugDraw3D::s_LineVertexArray = nullptr;
	Buffer *DebugDraw3D::s_LineVertexBuffer = nullptr;
	uint32_t DebugDraw3D::s_LineVertexCount = 0;
	LineVertex *DebugDraw3D::s_LineVertexBufferBase = nullptr;
	LineVertex *DebugDraw3D::s_LineVertexBufferPtr = nullptr;
	float DebugDraw3D::s_LineThickness = 3.0f;

	void DebugDraw3D::Init()
	{
		s_GLCache = GLCache::GetInstance();
		s_LineShader = ShaderLoader::LoadShader("DebugLine.glsl");
		s_LineVertexArray = new VertexArray();
		s_LineVertexBuffer = new Buffer(s_MaxLineVertices * sizeof(LineVertex)); // Dynamic GPU Data that can we can update with our current s_LineVertexBuffer
		s_LineVertexBuffer->SetComponentCount(3);
		s_LineVertexArray->AddBuffer(s_LineVertexBuffer, 0, 6 * sizeof(float), 0);
		s_LineVertexArray->AddBuffer(s_LineVertexBuffer, 1, 6 * sizeof(float), 3 * sizeof(float));
		s_LineVertexBufferBase = new LineVertex[s_MaxLineVertices];
	}

	void DebugDraw3D::BeginBatch()
	{
		s_LineVertexCount = 0;
		s_LineVertexBufferPtr = s_LineVertexBufferBase;
	}

	void DebugDraw3D::FlushBatch(ICamera *camera)
	{
		if (s_LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_LineVertexBufferPtr - (uint8_t*)s_LineVertexBufferBase);
			s_LineVertexBuffer->SetData(s_LineVertexBufferBase, dataSize);

			s_GLCache->SetLineSmooth(true);
			s_GLCache->SetLineWidth(s_LineThickness);
			s_GLCache->SetShader(s_LineShader);
			s_LineShader->SetUniform("viewProjection", camera->GetProjectionMatrix() * camera->GetViewMatrix());
			s_LineVertexArray->Bind();
			glDrawArrays(GL_LINES, 0, s_LineVertexCount);
		}
	}

	void DebugDraw3D::QueueLine(const glm::vec3 &lineStart, const glm::vec3 &lineEnd, const glm::vec3 &colour)
	{
		if (s_LineVertexCount >= s_MaxLineVertices)
		{
			ARC_ASSERT(false, "DebugDraw3D hit line limit, won't render additional lines. Up the limit if you need it!");
			return;
		}

		s_LineVertexBufferPtr->Position = lineStart;
		s_LineVertexBufferPtr->Colour = colour;
		s_LineVertexBufferPtr++;

		s_LineVertexBufferPtr->Position = lineEnd;
		s_LineVertexBufferPtr->Colour = colour;
		s_LineVertexBufferPtr++;

		s_LineVertexCount += 2;
	}

	void DebugDraw3D::QueueBox(const glm::vec3 &boxPos, const glm::vec3 &boxScale, const glm::vec3 &colour)
	{
		glm::vec3 boxScaleHalf = boxScale * 0.5f;
		QueueLine(glm::vec3(boxPos.x - boxScaleHalf.x, boxPos.y - boxScaleHalf.y, boxPos.z - boxScaleHalf.z), glm::vec3(boxPos.x + boxScaleHalf.x, boxPos.y - boxScaleHalf.y, boxPos.z - boxScaleHalf.z), colour);
		QueueLine(glm::vec3(boxPos.x - boxScaleHalf.x, boxPos.y - boxScaleHalf.y, boxPos.z - boxScaleHalf.z), glm::vec3(boxPos.x - boxScaleHalf.x, boxPos.y + boxScaleHalf.y, boxPos.z - boxScaleHalf.z), colour);
		QueueLine(glm::vec3(boxPos.x + boxScaleHalf.x, boxPos.y - boxScaleHalf.y, boxPos.z - boxScaleHalf.z), glm::vec3(boxPos.x + boxScaleHalf.x, boxPos.y + boxScaleHalf.y, boxPos.z - boxScaleHalf.z), colour);
		QueueLine(glm::vec3(boxPos.x - boxScaleHalf.x, boxPos.y + boxScaleHalf.y, boxPos.z - boxScaleHalf.z), glm::vec3(boxPos.x + boxScaleHalf.x, boxPos.y + boxScaleHalf.y, boxPos.z - boxScaleHalf.z), colour);

		QueueLine(glm::vec3(boxPos.x - boxScaleHalf.x, boxPos.y - boxScaleHalf.y, boxPos.z + boxScaleHalf.z), glm::vec3(boxPos.x + boxScaleHalf.x, boxPos.y - boxScaleHalf.y, boxPos.z + boxScaleHalf.z), colour);
		QueueLine(glm::vec3(boxPos.x - boxScaleHalf.x, boxPos.y - boxScaleHalf.y, boxPos.z + boxScaleHalf.z), glm::vec3(boxPos.x - boxScaleHalf.x, boxPos.y + boxScaleHalf.y, boxPos.z + boxScaleHalf.z), colour);
		QueueLine(glm::vec3(boxPos.x + boxScaleHalf.x, boxPos.y - boxScaleHalf.y, boxPos.z + boxScaleHalf.z), glm::vec3(boxPos.x + boxScaleHalf.x, boxPos.y + boxScaleHalf.y, boxPos.z + boxScaleHalf.z), colour);
		QueueLine(glm::vec3(boxPos.x - boxScaleHalf.x, boxPos.y + boxScaleHalf.y, boxPos.z + boxScaleHalf.z), glm::vec3(boxPos.x + boxScaleHalf.x, boxPos.y + boxScaleHalf.y, boxPos.z + boxScaleHalf.z), colour);

		QueueLine(glm::vec3(boxPos.x - boxScaleHalf.x, boxPos.y - boxScaleHalf.y, boxPos.z - boxScaleHalf.z), glm::vec3(boxPos.x - boxScaleHalf.x, boxPos.y - boxScaleHalf.y, boxPos.z + boxScaleHalf.z), colour);
		QueueLine(glm::vec3(boxPos.x + boxScaleHalf.x, boxPos.y - boxScaleHalf.y, boxPos.z - boxScaleHalf.z), glm::vec3(boxPos.x + boxScaleHalf.x, boxPos.y - boxScaleHalf.y, boxPos.z + boxScaleHalf.z), colour);

		QueueLine(glm::vec3(boxPos.x - boxScaleHalf.x, boxPos.y + boxScaleHalf.y, boxPos.z - boxScaleHalf.z), glm::vec3(boxPos.x - boxScaleHalf.x, boxPos.y + boxScaleHalf.y, boxPos.z + boxScaleHalf.z), colour);
		QueueLine(glm::vec3(boxPos.x + boxScaleHalf.x, boxPos.y + boxScaleHalf.y, boxPos.z - boxScaleHalf.z), glm::vec3(boxPos.x + boxScaleHalf.x, boxPos.y + boxScaleHalf.y, boxPos.z + boxScaleHalf.z), colour);
	}

	void DebugDraw3D::QueueBox(const glm::mat4 &transform, const glm::vec3 &colour)
	{

	}

	void DebugDraw3D::SetLineThickness(float thickness)
	{
		s_LineThickness = thickness;
	}
}
