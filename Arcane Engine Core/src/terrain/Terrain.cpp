#include "pch.h"
#include "Terrain.h"

namespace arcane {

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
			Logger::getInstance().error("logged_files/terrain_creation.txt", "terrain initialization", "Can't use a heightmap with a different width and height");
			return;
		}

		// Map Information
		m_VertexSideCount = mapWidth;
		m_TerrainSize = 4;
		m_HeightMapScale = 220;

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
		m_Textures[0] = TextureLoader::load2DTexture(std::string("res/terrain/grass.png"), true);
		m_Textures[1] = TextureLoader::load2DTexture(std::string("res/terrain/dirt.png"), true);
		m_Textures[2] = TextureLoader::load2DTexture(std::string("res/terrain/sand.png"), true);
		m_Textures[3] = TextureLoader::load2DTexture(std::string("res/terrain/stone.png"), true);
		m_Textures[4] = TextureLoader::load2DTexture(std::string("res/terrain/blendMap.png"), false);
		m_Textures[5] = TextureLoader::load2DTexture(std::string("res/terrain/grass_normal.png"), false);
		m_Textures[6] = TextureLoader::load2DTexture(std::string("res/terrain/dirt_normal.png"), false);
		m_Textures[7] = TextureLoader::load2DTexture(std::string("res/terrain/sand_normal.png"), false);
		m_Textures[8] = TextureLoader::load2DTexture(std::string("res/terrain/stone_normal.png"), false);

		m_Mesh = new Mesh(positions, uvs, normals, indices);
		m_Mesh->LoadData(true);
	}

	Terrain::~Terrain() {
		delete m_Mesh;
	}

	void Terrain::Draw(Shader *shader, RenderPassType pass) const {
		// Texture unit 0 is reserved for the shadowmap
		if (pass != RenderPassType::ShadowmapPassType) {
			m_Textures[0]->bind(1);
			shader->setUniform1i("material.texture_diffuse1", 1);
			m_Textures[1]->bind(2);
			shader->setUniform1i("material.texture_diffuse2", 2);
			m_Textures[2]->bind(3);
			shader->setUniform1i("material.texture_diffuse3", 3);
			m_Textures[3]->bind(4);
			shader->setUniform1i("material.texture_diffuse4", 4);
			m_Textures[4]->bind(5);
			shader->setUniform1i("material.blendmap", 5);
			m_Textures[5]->bind(6);
			shader->setUniform1i("material.texture_normal1", 6);
			m_Textures[6]->bind(7);
			shader->setUniform1i("material.texture_normal2", 7);
			m_Textures[7]->bind(8);
			shader->setUniform1i("material.texture_normal3", 8);
			m_Textures[8]->bind(9);
			shader->setUniform1i("material.texture_normal4", 9);
		}

		shader->setUniformMat4("model", m_ModelMatrix);
		m_Mesh->Draw();
	}

	bool Terrain::checkPointForIntersection(const glm::vec3& worldPos, glm::vec3& outIntersectionPoint) {
		// Get the closest vertex on the terrain (relative to the xz position)
		glm::vec3 terrainPos = sampleHeightfieldBilinear(worldPos);

		if (glm::length2(worldPos - terrainPos) < 1.2f) {
			outIntersectionPoint = terrainPos;
			return true;
		}

		return false;
	}

	// Nearest Neighbor sample for the terrain's vertex position
	glm::vec3& Terrain::sampleHeightfieldNearest(const glm::vec3& worldPos) {
		glm::vec2 terrainXZ = glm::vec2(clip(worldPos.x / m_TerrainSize, 0.0f, (float)m_VertexSideCount - 1.0f), clip(worldPos.z / m_TerrainSize, 0.0f, (float)m_VertexSideCount - 1.0f));
		return m_Mesh->GetPositions()[(int)terrainXZ.x + ((int)terrainXZ.y * m_VertexSideCount)];
	}

	// Bilinear sampling for the terrain's vertex position
	glm::vec3& Terrain::sampleHeightfieldBilinear(const glm::vec3& worldPos) {
		// Get the weights for the bilinear filter
		glm::vec2 terrainXZ = glm::vec2(worldPos.x / m_TerrainSize, worldPos.z / m_TerrainSize);
		float xFrac = terrainXZ.x - (int)terrainXZ.x;
		float zFrac = terrainXZ.y - (int)terrainXZ.y;

		// Get the values that should be lerped
		glm::vec3 topLeft = sampleHeightfieldNearest(worldPos);
		glm::vec3 topRight = sampleHeightfieldNearest(worldPos + glm::vec3(m_TerrainSize, 0.0f, 0.0f));
		glm::vec3 bottomLeft = sampleHeightfieldNearest(worldPos + glm::vec3(0.0f, 0.0f, m_TerrainSize));
		glm::vec3 bottomRight = sampleHeightfieldNearest(worldPos + glm::vec3(m_TerrainSize, 0.0f, m_TerrainSize));

		// Do the bilinear filtering
		glm::vec3 terrainPos = glm::mix(glm::mix(topLeft, topRight, xFrac), glm::mix(bottomLeft, bottomRight, xFrac), zFrac);
		return terrainPos;
	}

	float Terrain::clip(float n, float lower, float upper) {
		return std::max(lower, std::min(n, upper));
	}

	// Bilinear filtering for the terrain's normal
	glm::vec3 Terrain::calculateNormal(unsigned int x, unsigned int z, unsigned char *heightMapData) {
		float heightR = getVertexHeight(x + 1, z    , heightMapData);
		float heightL = getVertexHeight(x - 1, z    , heightMapData);
		float heightU = getVertexHeight(x    , z + 1, heightMapData);
		float heightD = getVertexHeight(x    , z - 1, heightMapData);
		
		glm::vec3 normal(heightL - heightR, 2.0f, heightD - heightU);
		normal = glm::normalize(normal);

		return normal;
	}

	float Terrain::getVertexHeight(unsigned int x, unsigned int z, unsigned char *heightMapData) {
		if (x < 0 || x >= m_VertexSideCount || z < 0 || z >= m_VertexSideCount) {
			return 0.0f;
		}

		// Normalize height to [0, 1] then multiply it by the height map scale
		return (heightMapData[x + (z * m_VertexSideCount)] / 255.0f) * m_HeightMapScale;
	}

}
