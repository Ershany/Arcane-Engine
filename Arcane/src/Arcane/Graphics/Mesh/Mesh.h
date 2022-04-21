#pragma once

#include <Arcane/Graphics/Mesh/Material.h>

namespace Arcane
{
	class Model;

	class Mesh {
		friend Model;
	public:
		Mesh();
		Mesh(std::vector<glm::vec3> &positions, std::vector<unsigned int> &indices);
		Mesh(std::vector<glm::vec3> &positions, std::vector<glm::vec2> &uvs, std::vector<unsigned int> &indices);
		Mesh(std::vector<glm::vec3> &positions, std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals, std::vector<unsigned int> &indices);
		Mesh(std::vector<glm::vec3> &positions, std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals, std::vector<glm::vec3> &tangents, std::vector<glm::vec3> &bitangents, std::vector<unsigned int> &indices);

		// Commits all of the buffers their attributes to the GPU driver
		void LoadData(bool interleaved = true);

		void Draw() const;

		inline void SetPositions(std::vector<glm::vec3> &positions) { m_Positions = positions; }
		inline void SetUVs(std::vector<glm::vec2> &uvs) { m_UVs = uvs; }
		inline void SetNormals(std::vector<glm::vec3> &normals) { m_Normals = normals; }
		inline void SetTangents(std::vector<glm::vec3> &tangents) { m_Tangents = tangents; }
		inline void SetBitangents(std::vector<glm::vec3> &bitangents) { m_Bitangents = bitangents; }
		inline void SetIndices(std::vector<unsigned int> &indices) { m_Indices = indices; }

		inline Material& GetMaterial() { return m_Material; }
	protected:
		unsigned int m_VAO, m_VBO, m_IBO;
		Material m_Material;

		std::vector<glm::vec3> m_Positions;
		std::vector<glm::vec2> m_UVs;
		std::vector<glm::vec3> m_Normals;
		std::vector<glm::vec3> m_Tangents;
		std::vector<glm::vec3> m_Bitangents;

		std::vector<unsigned int> m_Indices;
	};
}
