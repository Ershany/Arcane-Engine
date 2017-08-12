#include "Terrain.h"

#include "../platform/OpenGL/Utility.h"
#include "../utils/Logger.h"

namespace arcane { namespace terrain {

	Terrain::Terrain(glm::vec3 &worldPosition) : m_Position(worldPosition)
	{
		m_ModelMatrix = glm::translate(m_ModelMatrix, worldPosition);

		// Requirements to generate a mesh
		std::vector<graphics::Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<graphics::Texture> textures;

		// Height map
		GLint mapWidth, mapHeight;
		unsigned char *heightMapImage = stbi_load("res/terrain/heightMap.png", &mapWidth, &mapHeight, 0, SOIL_LOAD_L);
		if (mapWidth != mapHeight) {
			std::cout << "ERROR: Can't use a heightmap with a different width and height" << std::endl;
			utils::Logger::getInstance().error("logged_files/terrain_creation.txt", "terrain initialization", "Can't use a heightmap with a different width and height");
			return;
		}

		// Map Information
		m_VertexSideCount = mapWidth;
		m_TerrainSize = 4;
		m_HeightMapScale = 100;

		// Vertex generation
		for (GLuint z = 0; z < m_VertexSideCount; z++) {
			for (GLuint x = 0; x < m_VertexSideCount; x++) {
				graphics::Vertex vertex;

				vertex.Position = glm::vec3(x * m_TerrainSize, getVertexHeight(x, z, heightMapImage), z * m_TerrainSize);
				vertex.Normal = calculateNormal(x, z, heightMapImage);
				vertex.TexCoords = glm::vec2((GLfloat)x / ((GLfloat)m_VertexSideCount - 1.0f), (GLfloat)z / ((GLfloat)m_VertexSideCount - 1.0f));

				vertices.push_back(vertex);
			}
		}
		stbi_image_free(heightMapImage);

		// Indices generation (ccw winding order for consistency which will allow back face culling)
		for (GLuint height = 0; height < m_VertexSideCount - 1; ++height) {
			for (GLuint width = 0; width < m_VertexSideCount - 1; ++width) {
				// Triangle 1
				indices.push_back(width + (height * m_VertexSideCount));
				indices.push_back(1 + m_VertexSideCount + width + (height * m_VertexSideCount));
				indices.push_back(1 + width + (height * m_VertexSideCount));
				
				// Triangle 2
				indices.push_back(width + (height * m_VertexSideCount));
				indices.push_back(m_VertexSideCount + width + (height * m_VertexSideCount));
				indices.push_back(1 + m_VertexSideCount + width + (height * m_VertexSideCount));
			}
		}

		// Textures
		graphics::Texture texture;
		texture.id = opengl::Utility::loadTextureFromFile("res/terrain/grass.png");
		texture.type = "texture_diffuse";
		textures.push_back(texture);

		texture.id = opengl::Utility::loadTextureFromFile("res/terrain/dirt.png");
		texture.type = "texture_diffuse";
		textures.push_back(texture);

		texture.id = opengl::Utility::loadTextureFromFile("res/terrain/sand.png");
		texture.type = "texture_diffuse";
		textures.push_back(texture);

		texture.id = opengl::Utility::loadTextureFromFile("res/terrain/stone.png");
		texture.type = "texture_diffuse";
		textures.push_back(texture);

		texture.id = opengl::Utility::loadTextureFromFile("res/terrain/blendMap.png");
		texture.type = "texture_diffuse";
		textures.push_back(texture);


		m_Mesh = new graphics::Mesh(vertices, indices, textures);
	}

	Terrain::~Terrain() {
		delete m_Mesh;
	}

	void Terrain::Draw(graphics::Shader &shader) const {
		shader.setUniformMat4("model", m_ModelMatrix);
		m_Mesh->Draw(shader);
	}

	glm::vec3 Terrain::calculateNormal(int x, int z, unsigned char *heightMapData) {
		GLfloat heightR = getVertexHeight(x + 1, z    , heightMapData);
		GLfloat heightL = getVertexHeight(x - 1, z    , heightMapData);
		GLfloat heightU = getVertexHeight(x    , z + 1, heightMapData);
		GLfloat heightD = getVertexHeight(x    , z - 1, heightMapData);
		
		glm::vec3 normal(heightL - heightR, 2.0f, heightD - heightU);
		normal = glm::normalize(normal);

		return normal;
	}

	GLfloat Terrain::getVertexHeight(int x, int z, unsigned char *heightMapData) {
		if (x < 0 || x >= m_VertexSideCount || z < 0 || z >= m_VertexSideCount) {
			return 0.0f;
		}

		// Normalize height to [-1, 1] then multiply it by the height map scale
		return ((heightMapData[x + (z * m_VertexSideCount)] / 127.5f) - 1) * m_HeightMapScale;
	}

} }