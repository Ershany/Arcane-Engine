#pragma once
#ifndef BUFFER_H
#define BUFFER_H

namespace Arcane
{
	class Buffer
	{
	public:
		Buffer();
		Buffer(uint32_t size); // Can be used to allocate a big buffer then you can use SetData to vary its size
		Buffer(float *data, int amount, unsigned int componentCount);
		~Buffer();

		// Should use one, or the other depending on how this buffer is being used (statically vs dynamically which is 'Load()' vs 'SetData()' respectively)
		void SetData(const void *data, uint32_t size);
		void Load(float *data, int amount, unsigned int componentCount);

		void Bind() const;
		void Unbind() const;

		inline void SetComponentCount(unsigned int count) { m_ComponentCount = count; }
		inline unsigned int GetComponentCount() const { return m_ComponentCount; }
	private:
		unsigned int m_BufferID;
		unsigned int m_ComponentCount;
	};
}
#endif
