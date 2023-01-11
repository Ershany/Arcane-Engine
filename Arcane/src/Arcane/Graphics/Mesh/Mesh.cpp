#include "arcpch.h"
#include "Mesh.h"

#include <Arcane/Platform/OpenGL/IndexBuffer.h>
#include <Arcane/Platform/OpenGL/VertexArray.h>

namespace Arcane
{
	Mesh::Mesh() : m_VAO(0), m_VBO(0), m_IBO(0) {}

	Mesh::Mesh(std::vector<glm::vec3> &positions, std::vector<unsigned int> &indices)
		: m_Positions(positions), m_Indices(indices), m_VAO(0), m_VBO(0), m_IBO(0) {}

	Mesh::Mesh(std::vector<glm::vec3> &positions, std::vector<glm::vec2> &uvs, std::vector<unsigned int> &indices)
		: m_Positions(positions), m_UVs(uvs), m_Indices(indices), m_VAO(0), m_VBO(0), m_IBO(0) {}

	Mesh::Mesh(std::vector<glm::vec3> &positions, std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals, std::vector<unsigned int> &indices)
		: m_Positions(positions), m_UVs(uvs), m_Normals(normals), m_Indices(indices), m_VAO(0), m_VBO(0), m_IBO(0) {}

	Mesh::Mesh(std::vector<glm::vec3> &positions, std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals, std::vector<glm::vec3> &tangents, std::vector<glm::vec3> &bitangents, std::vector<unsigned int> &indices)
		: m_Positions(positions), m_UVs(uvs), m_Normals(normals), m_Tangents(tangents), m_Bitangents(bitangents), m_Indices(indices), m_VAO(0), m_VBO(0), m_IBO(0) {}
 

	void Mesh::Draw() const {
		glBindVertexArray(m_VAO);
		if (m_Indices.size() > 0) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		else {
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_Positions.size()));
		}
		glBindVertexArray(0);
	}

	void Mesh::LoadData(bool interleaved) {
		// Check for possible mesh initialization errors
#ifdef ARC_DEV_BUILD
		{
			unsigned int vertexCount = static_cast<unsigned int>(m_Positions.size());

			if (vertexCount == 0)
				ARC_LOG_WARN("Mesh doesn't contain any vertices");

			if (m_UVs.size() != 0 && m_UVs.size() != vertexCount)
				ARC_LOG_WARN("Mesh UV count doesn't match the vertex count");
			if (m_Normals.size() != 0 && m_Normals.size() != vertexCount)
				ARC_LOG_WARN("Mesh Normal count doesn't match the vertex count");
			if (m_Tangents.size() != 0 && m_Tangents.size() != vertexCount)
				ARC_LOG_WARN("Mesh Tangent count doesn't match the vertex count");
			if (m_Bitangents.size() != 0 && m_Bitangents.size() != vertexCount)
				ARC_LOG_WARN("Mesh Bitangent count doesn't match the vertex count");
		}
#endif

		m_IsInterleaved = interleaved;

		// Compute the component count
		m_BufferComponentCount = 0;
		if (m_Positions.size() > 0)
			m_BufferComponentCount += 3;
		if (m_Normals.size() > 0)
			m_BufferComponentCount += 3;
		if (m_UVs.size() > 0)
			m_BufferComponentCount += 2;
		if (m_Tangents.size() > 0)
			m_BufferComponentCount += 3;
		if (m_Bitangents.size() > 0)
			m_BufferComponentCount += 3;

		// Pre-process the mesh data in the format that was specified
		m_BufferData.reserve(m_Positions.size() + m_Normals.size() + m_UVs.size() + m_Tangents.size() + m_Bitangents.size());
		if (interleaved) {
			for (unsigned int i = 0; i < m_Positions.size(); i++) {
				m_BufferData.push_back(m_Positions[i].x);
				m_BufferData.push_back(m_Positions[i].y);
				m_BufferData.push_back(m_Positions[i].z);
				if (m_Normals.size() > 0) {
					m_BufferData.push_back(m_Normals[i].x);
					m_BufferData.push_back(m_Normals[i].y);
					m_BufferData.push_back(m_Normals[i].z);
				}
				if (m_UVs.size() > 0) {
					m_BufferData.push_back(m_UVs[i].x);
					m_BufferData.push_back(m_UVs[i].y);
				}
				if (m_Tangents.size() > 0) {
					m_BufferData.push_back(m_Tangents[i].x);
					m_BufferData.push_back(m_Tangents[i].y);
					m_BufferData.push_back(m_Tangents[i].z);
				}
				if (m_Bitangents.size() > 0) {
					m_BufferData.push_back(m_Bitangents[i].x);
					m_BufferData.push_back(m_Bitangents[i].y);
					m_BufferData.push_back(m_Bitangents[i].z);
				}
			}
		}
		else {
			for (unsigned int i = 0; i < m_Positions.size(); i++) {
				m_BufferData.push_back(m_Positions[i].x);
				m_BufferData.push_back(m_Positions[i].y);
				m_BufferData.push_back(m_Positions[i].z);
			}
			for (unsigned int i = 0; i < m_Normals.size(); i++) {
				m_BufferData.push_back(m_Normals[i].x);
				m_BufferData.push_back(m_Normals[i].y);
				m_BufferData.push_back(m_Normals[i].z);
			}
			for (unsigned int i = 0; i < m_UVs.size(); i++) {
				m_BufferData.push_back(m_UVs[i].x);
				m_BufferData.push_back(m_UVs[i].y);
			}
			for (unsigned int i = 0; i < m_Tangents.size(); i++) {
				m_BufferData.push_back(m_Tangents[i].x);
				m_BufferData.push_back(m_Tangents[i].y);
				m_BufferData.push_back(m_Tangents[i].z);
			}
			for (unsigned int i = 0; i < m_Bitangents.size(); i++) {
				m_BufferData.push_back(m_Bitangents[i].x);
				m_BufferData.push_back(m_Bitangents[i].y);
				m_BufferData.push_back(m_Bitangents[i].z);
			}
		}
	}

	void Mesh::GenerateGpuData()
	{
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_IBO);

		// Load data into the index buffer and vertex buffer
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, m_BufferData.size() * sizeof(float), &m_BufferData[0], GL_STATIC_DRAW);
		if (m_Indices.size() > 0)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);
		}

		// Setup the format for the VAO
		if (m_IsInterleaved)
		{
			size_t stride = m_BufferComponentCount * sizeof(float);
			size_t offset = 0;

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), (void*)offset);
			offset += 3 * sizeof(float);
			if (m_Normals.size() > 0)
			{
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), (void*)offset);
				offset += 3 * sizeof(float);
			}
			if (m_UVs.size() > 0)
			{
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), (void*)offset);
				offset += 2 * sizeof(float);
			}
			if (m_Tangents.size() > 0)
			{
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), (void*)offset);
				offset += 3 * sizeof(float);
			}
			if (m_Bitangents.size() > 0)
			{
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), (void*)offset);
				offset += 3 * sizeof(float);
			}
		}
		else
		{
			size_t offset = 0;

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
			offset += m_Positions.size() * 3 * sizeof(float);
			if (m_Normals.size() > 0)
			{
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
				offset += m_Normals.size() * 3 * sizeof(float);
			}
			if (m_UVs.size() > 0)
			{
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);
				offset += m_UVs.size() * 2 * sizeof(float);
			}
			if (m_Tangents.size() > 0)
			{
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
				offset += m_Tangents.size() * 3 * sizeof(float);
			}
			if (m_Bitangents.size() > 0)
			{
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
				offset += m_Bitangents.size() * 3 * sizeof(float);
			}
		}

		glBindVertexArray(0);
	}
}
