#pragma once

namespace arcane {

	class Buffer {
	private:
		unsigned int m_BufferID;
		unsigned int m_ComponentCount;
	public:
		Buffer();
		Buffer(float *data, int amount, unsigned int componentCount);
		~Buffer();

		void load(float *data, int amount, unsigned int componentCount);

		void bind() const;
		void unbind() const;

		inline unsigned int getComponentCount() const { return m_ComponentCount; }
	};

}
