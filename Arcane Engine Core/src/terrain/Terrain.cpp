#include "pch.h"
#include "Terrain.h"

namespace arcane {

	Terrain::Terrain(glm::vec3 &worldPosition) : m_Position(worldPosition)
	{
		m_ModelMatrix = glm::translate(m_ModelMatrix, worldPosition);

		// Height map
		int mapWidth, mapHeight;
		unsigned char *heightMapImage = stbi_load("res/terrain/heightMap.png", &mapWidth, &mapHeight, 0, SOIL_LOAD_L);
		if (mapWidth != mapHeight) {
			Logger::getInstance().error("logged_files/terrain_creation.txt", "terrain initialization", "Can't use a heightmap with a different width and height");
			return;
		}

		// Terrain information
		m_HeightfieldTextureSize = mapWidth;
		m_SideVertexCount = mapWidth * 0.25f;
		m_TerrainSizeXZ = 2048.0;
		m_TerrainSizeY = 400.0f;
		m_SpaceBetweenVertices = m_TerrainSizeXZ / (float)m_SideVertexCount;
		m_TerrainToHeightfieldTextureConversion = 1.0f / (m_TerrainSizeXZ / m_HeightfieldTextureSize);

		// Requirements to generate a mesh
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;
		positions.reserve(m_SideVertexCount * m_SideVertexCount);
		uvs.reserve(m_SideVertexCount * m_SideVertexCount);
		normals.reserve(m_SideVertexCount * m_SideVertexCount);
		indices.reserve(m_SideVertexCount * m_SideVertexCount * 6);

		// Vertex generation
		for (unsigned int z = 0; z < m_SideVertexCount; z++) {
			for (unsigned int x = 0; x < m_SideVertexCount; x++) {
				glm::vec2 positionXZ(x * m_SpaceBetweenVertices, z * m_SpaceBetweenVertices);

				positions.push_back(glm::vec3(positionXZ.x, sampleHeightfieldBilinear(positionXZ.x, positionXZ.y, heightMapImage), positionXZ.y));
				uvs.push_back(glm::vec2((float)x / (float)(m_SideVertexCount - 1), (float)z / (float)(m_SideVertexCount - 1)));
				normals.push_back(calculateNormal(positionXZ.x, positionXZ.y, heightMapImage));
			}
		}
		stbi_image_free(heightMapImage);

		// Indices generation (ccw winding order for consistency which will allow back face culling)
		for (unsigned int height = 0; height < m_SideVertexCount - 1; ++height) {
			for (unsigned int width = 0; width < m_SideVertexCount - 1; ++width) {
				// Triangle 1
				indices.push_back(width + (height * m_SideVertexCount));
				indices.push_back(1 + m_SideVertexCount + width + (height * m_SideVertexCount));
				indices.push_back(1 + width + (height * m_SideVertexCount));
				
				// Triangle 2
				indices.push_back(width + (height * m_SideVertexCount));
				indices.push_back(m_SideVertexCount + width + (height * m_SideVertexCount));
				indices.push_back(1 + m_SideVertexCount + width + (height * m_SideVertexCount));
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

		// Only set normal matrix for non shadowmap pass
		shader->setUniformMat4("model", m_ModelMatrix);
		m_Mesh->Draw();
	}

	// Bilinear filtering for the terrain's normal
	glm::vec3 Terrain::calculateNormal(float worldPosX, float worldPosZ, unsigned char *heightMapData) {
		float heightR = sampleHeightfieldNearest(worldPosX + m_SpaceBetweenVertices * 2, worldPosZ                         , heightMapData);
		float heightL = sampleHeightfieldNearest(worldPosX - m_SpaceBetweenVertices * 2, worldPosZ                         , heightMapData);
		float heightU = sampleHeightfieldNearest(worldPosX                         , worldPosZ + m_SpaceBetweenVertices * 2, heightMapData);
		float heightD = sampleHeightfieldNearest(worldPosX                         , worldPosZ - m_SpaceBetweenVertices * 2, heightMapData);
		
		glm::vec3 normal(heightL - heightR, 2.0f, heightD - heightU);
		normal = glm::normalize(normal);

		return normal;
	}

	float Terrain::sampleHeightfieldBilinear(float worldPosX, float worldPosZ, unsigned char *heightMapData) {
		// Calculate weights
		glm::vec2 weightsXZ = glm::vec2(worldPosX / m_SpaceBetweenVertices, worldPosZ / m_SpaceBetweenVertices);
		float xFrac = weightsXZ.x - (int)weightsXZ.x;
		float zFrac = weightsXZ.y - (int)weightsXZ.y;

		// Get the values that should be lerped between
		float topLeft = sampleHeightfieldNearest(worldPosX, worldPosZ, heightMapData);
		float topRight = sampleHeightfieldNearest(worldPosX + m_SpaceBetweenVertices, worldPosZ, heightMapData);
		float bottomLeft = sampleHeightfieldNearest(worldPosX, worldPosZ + m_SpaceBetweenVertices, heightMapData);
		float bottomRight = sampleHeightfieldNearest(worldPosX + m_SpaceBetweenVertices, worldPosZ + m_SpaceBetweenVertices, heightMapData);

		// Do the bilinear filtering
		float terrainHeight = glm::mix(glm::mix(topLeft, topRight, xFrac), glm::mix(bottomLeft, bottomRight, xFrac), zFrac);
		return terrainHeight;
	}

	float Terrain::sampleHeightfieldNearest(float worldPosX, float worldPosZ, unsigned char *heightMapData) {
		// Get the xz coordinates of the index after cutting off the decimal
		glm::vec2 terrainXZ = glm::vec2(clamp(worldPosX * m_TerrainToHeightfieldTextureConversion, 0.0f, (float)m_HeightfieldTextureSize - 0.0f), clamp(worldPosZ * m_TerrainToHeightfieldTextureConversion, 0.0f, (float)m_HeightfieldTextureSize - 1.0f));

		// Normalize height to [0, 1] then multiply it by the terrain's Y scale
		return (heightMapData[(unsigned int)terrainXZ.x + ((unsigned int)terrainXZ.y * m_HeightfieldTextureSize)] / 255.0f) * m_TerrainSizeY;
	}

	float Terrain::clamp(float n, float lower, float upper) {
		return std::max(lower, std::min(n, upper));
	}

}
