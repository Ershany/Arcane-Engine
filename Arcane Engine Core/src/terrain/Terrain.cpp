#include "Terrain.h"

#include "../platform/OpenGL/Utility.h"

namespace arcane { namespace terrain {

	Terrain::Terrain(glm::vec3 &worldPosition)
	{
		m_Position = worldPosition;
		m_VertexSideCount = 15;
		m_TerrainSize = 5;

		std::vector<graphics::Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<graphics::Texture> textures;
		for (unsigned int z = 0; z < m_VertexSideCount; z++) {
			for (unsigned int x = 0; x < m_VertexSideCount; x++) {
				graphics::Vertex vertex;

				vertex.Position = glm::vec3(x * m_TerrainSize, 0, z * m_TerrainSize);
				vertex.Normal = glm::vec3(0, 1, 0);
				vertex.TexCoords = glm::vec2(m_VertexSideCount * (GLfloat)x / ((GLfloat)m_VertexSideCount - 1.0f), m_VertexSideCount * (GLfloat)z / ((GLfloat)m_VertexSideCount - 1.0f));
				//vertex.TexCoords = glm::vec2((GLfloat)x / ((GLfloat)m_VertexSideCount - 1.0f), (GLfloat)z / ((GLfloat)m_VertexSideCount - 1.0f)); // ugly and stretched

				vertices.push_back(vertex);
			}
		}

		// Loop through the squares and break them up into triangles
		for (GLuint height = 0; height < m_VertexSideCount - 1; ++height) {
			for (GLuint width = 0; width < m_VertexSideCount - 1; ++width) {
				// Triangle 1
				indices.push_back(width + (height * m_VertexSideCount));
				indices.push_back(1 + width + (height * m_VertexSideCount));
				indices.push_back(1 + m_VertexSideCount + width + (height * m_VertexSideCount));

				// Triangle 2
				indices.push_back(width + (height * m_VertexSideCount));
				indices.push_back(m_VertexSideCount + width + (height * m_VertexSideCount));
				indices.push_back(1 + m_VertexSideCount + width + (height * m_VertexSideCount));
			}
		}

		graphics::Texture texture;
		texture.id = opengl::Utility::loadTextureFromFile("res/terrain/grass.png");
		texture.type = "texture_diffuse";
		textures.push_back(texture);



		m_Mesh = new graphics::Mesh(vertices, indices, textures);
	}

	Terrain::~Terrain() {
		delete m_Mesh;
	}

	void Terrain::Draw(graphics::Shader &shader) const {
		m_Mesh->Draw(shader);
	}

} }