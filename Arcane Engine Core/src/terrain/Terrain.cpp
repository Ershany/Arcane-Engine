#include "Terrain.h"

namespace arcane { namespace terrain {

	Terrain::Terrain(glm::vec3 &worldPosition)
	{
		m_Position = worldPosition;
		m_VertexSideCount = 4;
		m_TerrainSize = 3;

		std::vector<graphics::Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<graphics::Texture> textures;
		for (unsigned int z = 0; z < m_VertexSideCount; z++) {
			for (unsigned int x = 0; x < m_VertexSideCount; x++) {
				graphics::Vertex vertex;

				vertex.Position = glm::vec3(x * m_TerrainSize, 0, z * m_TerrainSize);
				vertex.Normal = glm::vec3(0, 1, 0);
				vertex.TexCoords = glm::vec2(0, 0);

				vertices.push_back(vertex);
			}
		}

		GLuint firstRowTriCount = 2 * (m_VertexSideCount - 1);
		GLuint triCount = firstRowTriCount + (m_VertexSideCount - 2) * firstRowTriCount;
		GLboolean temp = true;
		for (GLuint i = 0; i < triCount; i++) {
			if (temp) {
				indices.push_back(i);
				indices.push_back(i + 1);
				indices.push_back(i + m_VertexSideCount);
			}
			else {
				indices.push_back(i - m_VertexSideCount);
				indices.push_back(i + 1);
				indices.push_back(i);
			}
		}

		/*for (unsigned int z = 0; z < 2; z++) {
			for (unsigned int x = 0; x < 2; x++) {
			graphics::Vertex vertex;

			vertex.Position = glm::vec3(m_Position.x + x * m_TerrainSize, 0, m_Position.z + z * m_TerrainSize);
			vertex.Normal = glm::vec3(0, 1, 0);
			vertex.TexCoords = glm::vec2(0, 0);

			vertices.push_back(vertex);
			}
		}
		indices.push_back(0); indices.push_back(1); indices.push_back(2); indices.push_back(1); indices.push_back(3); indices.push_back(2);*/

		m_Mesh = new graphics::Mesh(vertices, indices, textures);
	}

	Terrain::~Terrain() {
		delete m_Mesh;
	}

	void Terrain::Draw(graphics::Shader &shader) const {
		m_Mesh->Draw(shader);
	}

} }