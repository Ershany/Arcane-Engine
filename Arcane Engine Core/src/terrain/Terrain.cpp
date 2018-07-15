#include "Terrain.h"

#include "../platform/OpenGL/Utility.h"
#include "../utils/Logger.h"

namespace arcane { namespace terrain {

	Terrain::Terrain(glm::vec3 &worldPosition) : m_Position(worldPosition)
	{
		m_ModelMatrix = glm::translate(m_ModelMatrix, worldPosition);

		// Requirements to generate a mesh
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		// Height map
		int mapWidth, mapHeight;
		unsigned char *heightMapImage = stbi_load("res/terrain/heightMap.png", &mapWidth, &mapHeight, 0, SOIL_LOAD_L);
		if (mapWidth != mapHeight) {
			std::cout << "ERROR: Can't use a heightmap with a different width and height" << std::endl;
			utils::Logger::getInstance().error("logged_files/terrain_creation.txt", "terrain initialization", "Can't use a heightmap with a different width and height");
			return;
		}

		// Map Information
		m_VertexSideCount = mapWidth;
		m_TerrainSize = 4;
		m_HeightMapScale = 150;

		// Vertex generation
		for (unsigned int z = 0; z < m_VertexSideCount; z++) {
			for (unsigned int x = 0; x < m_VertexSideCount; x++) {
				positions.push_back(glm::vec3(x * m_TerrainSize, getVertexHeight(x, z, heightMapImage), z * m_TerrainSize));
				uvs.push_back(glm::vec2((float)x / ((float)m_VertexSideCount - 1.0f), (float)z / ((float)m_VertexSideCount - 1.0f)));
				normals.push_back(calculateNormal(x, z, heightMapImage));
			}
		}
		stbi_image_free(heightMapImage);

		// Indices generation (ccw winding order for consistency which will allow back face culling)
		for (unsigned int height = 0; height < m_VertexSideCount - 1; ++height) {
			for (unsigned int width = 0; width < m_VertexSideCount - 1; ++width) {
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
		m_Textures[0] = texture;

		texture.id = opengl::Utility::loadTextureFromFile("res/terrain/dirt.png");
		texture.type = "texture_diffuse";
		m_Textures[1] = texture;

		texture.id = opengl::Utility::loadTextureFromFile("res/terrain/sand.png");
		texture.type = "texture_diffuse";
		m_Textures[2] = texture;

		texture.id = opengl::Utility::loadTextureFromFile("res/terrain/stone.png");
		texture.type = "texture_diffuse";
		m_Textures[3] = texture;

		texture.id = opengl::Utility::loadTextureFromFile("res/terrain/blendMap.png");
		texture.type = "texture_diffuse";
		m_Textures[4] = texture;

		m_Mesh = new graphics::Mesh(positions, uvs, normals, indices);
		m_Mesh->LoadData(true);
	}

	Terrain::~Terrain() {
		delete m_Mesh;
	}

	void Terrain::Draw(graphics::Shader &shader) const {
		glActiveTexture(GL_TEXTURE0);
		shader.setUniform1i("material.texture_diffuse1", 0);
		glBindTexture(GL_TEXTURE_2D, m_Textures[0].id);

		glActiveTexture(GL_TEXTURE1);
		shader.setUniform1i("material.texture_diffuse2", 1);
		glBindTexture(GL_TEXTURE_2D, m_Textures[1].id);

		glActiveTexture(GL_TEXTURE2);
		shader.setUniform1i("material.texture_diffuse3", 2);
		glBindTexture(GL_TEXTURE_2D, m_Textures[2].id);

		glActiveTexture(GL_TEXTURE3);
		shader.setUniform1i("material.texture_diffuse4", 3);
		glBindTexture(GL_TEXTURE_2D, m_Textures[3].id);

		glActiveTexture(GL_TEXTURE4);
		shader.setUniform1i("material.texture_diffuse5", 4);
		glBindTexture(GL_TEXTURE_2D, m_Textures[4].id);

		shader.setUniformMat4("model", m_ModelMatrix);
		m_Mesh->Draw();
	}

	glm::vec3 Terrain::calculateNormal(int x, int z, unsigned char *heightMapData) {
		float heightR = getVertexHeight(x + 1, z    , heightMapData);
		float heightL = getVertexHeight(x - 1, z    , heightMapData);
		float heightU = getVertexHeight(x    , z + 1, heightMapData);
		float heightD = getVertexHeight(x    , z - 1, heightMapData);
		
		glm::vec3 normal(heightL - heightR, 2.0f, heightD - heightU);
		normal = glm::normalize(normal);

		return normal;
	}

	float Terrain::getVertexHeight(int x, int z, unsigned char *heightMapData) {
		if (x < 0 || x >= m_VertexSideCount || z < 0 || z >= m_VertexSideCount) {
			return 0.0f;
		}

		// Normalize height to [-1, 1] then multiply it by the height map scale
		return ((heightMapData[x + (z * m_VertexSideCount)] / 127.5f) - 1) * m_HeightMapScale;
	}

} }