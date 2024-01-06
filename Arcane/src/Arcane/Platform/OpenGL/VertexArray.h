#pragma once
#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

namespace Arcane
{
	class Buffer;

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		// Function for automatically adding non-interleaved buffer data
		void AddBuffer(Buffer *buffer, int index, size_t stride = 0, size_t offset = 0);

		void Bind() const;
		void Unbind() const;
	private:
		unsigned int m_VertexArrayID;
		std::vector<Buffer*> m_Buffers;
	};
}
#endif
