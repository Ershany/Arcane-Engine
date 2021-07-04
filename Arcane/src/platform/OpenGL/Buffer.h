#pragma once

namespace Arcane
{
	class Buffer {
	public:
		Buffer();
		Buffer(float *data, int amount, unsigned int componentCount);
		~Buffer();

		void Load(float *data, int amount, unsigned int componentCount);

		void Bind() const;
		void Unbind() const;

		inline unsigned int GetComponentCount() const { return m_ComponentCount; }
	private:
		unsigned int m_BufferID;
		unsigned int m_ComponentCount;
	};
}
