#include "NewMesh.h"

namespace arcane { namespace graphics {

	NewMesh::NewMesh() {}

	NewMesh::NewMesh(std::vector<glm::vec3> positions, std::vector<unsigned int> indices)
		: m_Positions(positions), m_Indices(indices) {}

	NewMesh::NewMesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uvs, std::vector<unsigned int> indices)
		: m_Positions(positions), m_UVs(uvs), m_Indices(indices) {}

	NewMesh::NewMesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uvs, std::vector<glm::vec3> normals, std::vector<unsigned int> indices)
		: m_Positions(positions), m_UVs(uvs), m_Normals(normals), m_Indices(indices) {}

	NewMesh::NewMesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uvs, std::vector<glm::vec3> normals, std::vector<glm::vec3> tangents, std::vector<glm::vec3> bitangents, std::vector<unsigned int> indices)
		: m_Positions(positions), m_UVs(uvs), m_Normals(normals), m_Tangents(tangents), m_Bitangents(bitangents) {}
 

	void NewMesh::Draw() const {
		m_VAO.bind();
		m_EBO.bind();
		glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
		m_EBO.unbind();
		m_VAO.unbind();
	}

	void NewMesh::LoadData(bool interleaved) {
		m_VAO.bind();

		// Check for possible mesh initialization errors
		{
			unsigned int vertexCount = m_Positions.size();

			if (vertexCount == 0)
				utils::Logger::getInstance().error("logged_files/mesh_creation.txt", "Mesh Creation", "Mesh doesn't contain any vertices");

			if (m_UVs.size() != 0 && m_UVs.size() != vertexCount)
				utils::Logger::getInstance().error("logged_files/mesh_creation.txt", "Mesh Creation", "Mesh UV count doesn't match the vertex count");
			if (m_Normals.size() != 0 && m_Normals.size() != vertexCount)
				utils::Logger::getInstance().error("logged_files/mesh_creation.txt", "Mesh Creation", "Mesh Normal count doesn't match the vertex count");
			if (m_Tangents.size() != 0 && m_Tangents.size() != vertexCount)
				utils::Logger::getInstance().error("logged_files/mesh_creation.txt", "Mesh Creation", "Mesh Tangent count doesn't match the vertex count");
			if (m_Bitangents.size() != 0 && m_Bitangents.size() != vertexCount)
				utils::Logger::getInstance().error("logged_files/mesh_creation.txt", "Mesh Creation", "Mesh Bitangent count doesn't match the vertex count");
		}

		// Preprocess the mesh data in the format that was specified
		std::vector<float> data;
		if (interleaved) {
			for (unsigned int i = 0; i < m_Positions.size(); i++) {
				data.push_back(m_Positions[i].x);
				data.push_back(m_Positions[i].y);
				data.push_back(m_Positions[i].z);
				if (m_UVs.size() > 0) {
					data.push_back(m_UVs[i].x);
					data.push_back(m_UVs[i].y);
				}
				if (m_Normals.size() > 0) {
					data.push_back(m_Normals[i].x);
					data.push_back(m_Normals[i].y);
					data.push_back(m_Normals[i].z);
				}
				if (m_Tangents.size() > 0) {
					data.push_back(m_Tangents[i].x);
					data.push_back(m_Tangents[i].y);
					data.push_back(m_Tangents[i].z);
				}
				if (m_Bitangents.size() > 0) {
					data.push_back(m_Bitangents[i].x);
					data.push_back(m_Bitangents[i].y);
					data.push_back(m_Bitangents[i].z);
				}
			}
		}
		else {
			for (unsigned int i = 0; i < m_Positions.size(); i++) {
				data.push_back(m_Positions[i].x);
				data.push_back(m_Positions[i].y);
				data.push_back(m_Positions[i].z);
			}
			for (unsigned int i = 0; i < m_UVs.size(); i++) {
				data.push_back(m_UVs[i].x);
				data.push_back(m_UVs[i].y);
			}
			for (unsigned int i = 0; i < m_Normals.size(); i++) {
				data.push_back(m_Normals[i].x);
				data.push_back(m_Normals[i].y);
				data.push_back(m_Normals[i].z);
			}
			for (unsigned int i = 0; i < m_Tangents.size(); i++) {
				data.push_back(m_Tangents[i].x);
				data.push_back(m_Tangents[i].y);
				data.push_back(m_Tangents[i].z);
			}
			for (unsigned int i = 0; i < m_Bitangents.size(); i++) {
				data.push_back(m_Bitangents[i].x);
				data.push_back(m_Bitangents[i].y);
				data.push_back(m_Bitangents[i].z);
			}
		}

		// Compute the component count
		unsigned int bufferComponentCount = 0;
		if (m_Positions.size() > 0)
			bufferComponentCount += 3;
		if (m_UVs.size() > 0)
			bufferComponentCount += 2;
		if (m_Normals.size() > 0)
			bufferComponentCount += 3;
		if (m_Tangents.size() > 0)
			bufferComponentCount += 3;
		if (m_Bitangents.size() > 0)
			bufferComponentCount += 3;

		// Load data into the index buffer and vertex buffer
		m_VAO.bind();
		m_VBO.load(&data[0], data.size(), bufferComponentCount);
		if (m_Indices.size() > 0) {
			m_EBO.load(&m_Indices[0], m_Indices.size());
		}

		// Setup the format for the VAO
		if (interleaved) {
			size_t stride = bufferComponentCount * sizeof(float);
			size_t offset = 0;

			m_VAO.addVertexAttribArrayPointer(0, 3, stride, offset);
			offset += 3 * sizeof(float);
			if (m_UVs.size() > 0) {
				m_VAO.addVertexAttribArrayPointer(1, 2, stride, offset);
				offset += 2 * sizeof(float);
			}
			if (m_Normals.size() > 0) {
				m_VAO.addVertexAttribArrayPointer(2, 3, stride, offset);
				offset += 3 * sizeof(float);
			}
			if (m_Tangents.size() > 0) {
				m_VAO.addVertexAttribArrayPointer(3, 3, stride, offset);
				offset += 3 * sizeof(float);
			}
			if (m_Bitangents.size() > 0) {
				m_VAO.addVertexAttribArrayPointer(4, 3, stride, offset);
				offset += 3 * sizeof(float);
			}
		}
		else {
			size_t offset = 0;

			m_VAO.addVertexAttribArrayPointer(0, 3, 0, offset);
			offset += m_Positions.size() * sizeof(float);
			if (m_UVs.size() > 0) {
				m_VAO.addVertexAttribArrayPointer(1, 2, 0, offset);
				offset += m_UVs.size() * sizeof(float);
			}
			if (m_Normals.size() > 0) {
				m_VAO.addVertexAttribArrayPointer(2, 3, 0, offset);
				offset += m_Normals.size() * sizeof(float);
			}
			if (m_Tangents.size() > 0) {
				m_VAO.addVertexAttribArrayPointer(3, 3, 0, offset);
				offset += m_Tangents.size() * sizeof(float);
			}
			if (m_Bitangents.size() > 0) {
				m_VAO.addVertexAttribArrayPointer(4, 3, 0, offset);
				offset += m_Bitangents.size() * sizeof(float);
			}
		}

		m_VAO.unbind();
	}

} }