#pragma once

#include "Buffer.h"

namespace Arcane
{
	class VertexArray {
	public:
		VertexArray();
		~VertexArray();

		// Function for automatically adding non-interleaved buffer data
		void AddBuffer(Buffer *buffer, int index);

		void Bind() const;
		void Unbind() const;
	private:
		unsigned int m_VertexArrayID;
		std::vector<Buffer*> m_Buffers;
	};
}
