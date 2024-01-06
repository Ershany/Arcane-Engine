#include "arcpch.h"
#include "Buffer.h"

namespace Arcane
{
	Buffer::Buffer() : m_ComponentCount(0)
	{
		glGenBuffers(1, &m_BufferID);
	}

	// Can be used to allocate a big buffer then you can use SetData to vary its size
	Buffer::Buffer(uint32_t size)
	{
		glGenBuffers(1, &m_BufferID);
		Bind();
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW); // Dynamic since this will change size and what were drawing
	}

	Buffer::Buffer(float *data, int amount, unsigned int componentCount)
	{
		glGenBuffers(1, &m_BufferID);
		Load(data, amount, componentCount);
	}

	Buffer::~Buffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	void Buffer::SetData(const void *data, uint32_t size)
	{
		Bind();
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	void Buffer::Load(float *data, int amount, unsigned int componentCount)
	{
		m_ComponentCount = componentCount;

		Bind();
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(float), data, GL_STATIC_DRAW);
	}

	void Buffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}

	void Buffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
