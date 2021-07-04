#pragma once

namespace Arcane
{
	class IndexBuffer {
	public:
		IndexBuffer();
		IndexBuffer(unsigned int *data, int amount);
		~IndexBuffer();

		void Load(unsigned int *data, int amount);

		void Bind() const;
		void Unbind() const;

		inline int GetCount() const { return m_Count; }
	private:
		unsigned int m_BufferID;
		int m_Count;
	};
}
