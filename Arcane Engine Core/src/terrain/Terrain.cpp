#include "pch.h"
#include "Terrain.h"

namespace arcane {

	Terrain::Terrain(glm::vec3 &worldPosition) : m_Position(worldPosition)
	{
		m_GLCache = GLCache::getInstance();

		m_ModelMatrix = glm::translate(m_ModelMatrix, worldPosition);

		// Height map
		int mapWidth, mapHeight;
		unsigned char *heightMapImage = stbi_load("res/terrain/heightMap.png", &mapWidth, &mapHeight, 0, SOIL_LOAD_L);
		if (mapWidth != mapHeight) {
			Logger::getInstance().error("logged_files/terrain_creation.txt", "terrain initialization", "Can't use a heightmap with a different width and height");
			return;
		}

		// Terrain information
		m_TextureTilingAmount = 64;
		m_HeightfieldTextureSize = mapWidth;
		m_SideVertexCount = (unsigned int)(mapWidth * 0.25f);
		m_TerrainSizeXZ = 2048.0;
		m_TerrainSizeY = 400.0f;
		m_SpaceBetweenVertices = m_TerrainSizeXZ / (float)m_SideVertexCount;
		m_TerrainToHeightfieldTextureConversion = 1.0f / (m_TerrainSizeXZ / m_HeightfieldTextureSize);

		// Requirements to generate a mesh
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> tangents(m_SideVertexCount * m_SideVertexCount, glm::vec3(0.0f, 0.0f, 0.0f));
		std::vector<glm::vec3> bitangents(m_SideVertexCount * m_SideVertexCount, glm::vec3(0.0f, 0.0f, 0.0f));
		std::vector<unsigned int> indices;
		positions.reserve(m_SideVertexCount * m_SideVertexCount);
		uvs.reserve(m_SideVertexCount * m_SideVertexCount);
		normals.reserve(m_SideVertexCount * m_SideVertexCount);
		indices.reserve((m_SideVertexCount - 1) * (m_SideVertexCount - 1) * 6);

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
		for (unsigned int height = 0; height < m_SideVertexCount - 1; height++) {
			for (unsigned int width = 0; width < m_SideVertexCount - 1; width++) {
				unsigned int indexTL = width + (height * m_SideVertexCount);
				unsigned int indexTR = 1 + width + (height * m_SideVertexCount);
				unsigned int indexBL = m_SideVertexCount + width + (height * m_SideVertexCount);
				unsigned int indexBR = 1 + m_SideVertexCount + width + (height * m_SideVertexCount);

				// Triangle 1
				indices.push_back(indexTL);
				indices.push_back(indexBR);
				indices.push_back(indexTR);
				
				// Triangle 2
				indices.push_back(indexTL);
				indices.push_back(indexBL);
				indices.push_back(indexBR);

				// Triangle 1 tangent + bitangent calculations
				glm::vec3 &v0 = positions[indexTL];
				glm::vec3 &v1 = positions[indexBR];
				glm::vec3 &v2 = positions[indexTR];
				glm::vec2 &uv0 = uvs[indexTL];
				glm::vec2 &uv1 = uvs[indexBR];
				glm::vec2 &uv2 = uvs[indexTR];
				glm::vec3 deltaPos1 = v1 - v0;
				glm::vec3 deltaPos2 = v2 - v0;
				glm::vec2 deltaUV1 = uv1 - uv0;
				glm::vec2 deltaUV2 = uv2 - uv0;
				float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
				glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
				tangents[indexTL] += tangent;
				tangents[indexBR] += tangent;
				tangents[indexTR] += tangent;

				// Triangle 2 tangent + bitangent calculations
				v0 = positions[indexTL];
				v1 = positions[indexBR];
				v2 = positions[indexTR];
				uv0 = uvs[indexTL];
				uv1 = uvs[indexBR];
				uv2 = uvs[indexTR];
				deltaPos1 = v1 - v0;
				deltaPos2 = v2 - v0;
				deltaUV1 = uv1 - uv0;
				deltaUV2 = uv2 - uv0;
				r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
				tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
				tangents[indexTL] += tangent;
				tangents[indexBL] += tangent;
				tangents[indexBR] += tangent;
			}
		}

		// Gram-Schmidt Process for fixing up the tangent vector and calculating the bitangent
		for (unsigned int i = 0; i < tangents.size(); i++) {
			const glm::vec3 &normal = normals[i];
			glm::vec3 tangent = glm::normalize(tangents[i]);

			tangent = glm::normalize(tangent - glm::dot(tangent, normal) * normal);
			glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));

			tangents[i] = tangent;
			bitangents[i] = bitangent;
		}

		// Textures
		TextureSettings srgbTextureSettings;
		srgbTextureSettings.IsSRGB = true;

		m_Textures[0] = TextureLoader::load2DTexture(std::string("res/terrain/grass/grassAlbedo.tga"), &srgbTextureSettings);
		m_Textures[1] = TextureLoader::load2DTexture(std::string("res/terrain/dirt/dirtAlbedo.tga"), &srgbTextureSettings);
		m_Textures[2] = TextureLoader::load2DTexture(std::string("res/terrain/branches/branchesAlbedo.tga"), &srgbTextureSettings);
		m_Textures[3] = TextureLoader::load2DTexture(std::string("res/terrain/rock/rockAlbedo.tga"), &srgbTextureSettings);

		m_Textures[4] = TextureLoader::load2DTexture(std::string("res/terrain/grass/grassNormal.tga"));
		m_Textures[5] = TextureLoader::load2DTexture(std::string("res/terrain/dirt/dirtNormal.tga"));
		m_Textures[6] = TextureLoader::load2DTexture(std::string("res/terrain/branches/branchesNormal.tga"));
		m_Textures[7] = TextureLoader::load2DTexture(std::string("res/terrain/rock/rockNormal.tga"));

		// We do not want these texture treated as one channel so store it as RGB
		TextureSettings textureSettings;
		textureSettings.TextureFormat = GL_RGB;

		m_Textures[8] = TextureLoader::load2DTexture(std::string("res/terrain/grass/grassRoughness.tga"), &textureSettings);
		m_Textures[9] = TextureLoader::load2DTexture(std::string("res/terrain/dirt/dirtRoughness.tga"), &textureSettings);
		m_Textures[10] = TextureLoader::load2DTexture(std::string("res/terrain/branches/branchesRoughness.tga"), &textureSettings);
		m_Textures[11] = TextureLoader::load2DTexture(std::string("res/terrain/rock/rockRoughness.tga"), &textureSettings);

		m_Textures[12] = TextureLoader::load2DTexture(std::string("res/terrain/grass/grassMetallic.tga"), &textureSettings);
		m_Textures[13] = TextureLoader::load2DTexture(std::string("res/terrain/dirt/dirtMetallic.tga"), &textureSettings);
		m_Textures[14] = TextureLoader::load2DTexture(std::string("res/terrain/branches/branchesMetallic.tga"), &textureSettings);
		m_Textures[15] = TextureLoader::load2DTexture(std::string("res/terrain/rock/rockMetallic.tga"), &textureSettings);

		m_Textures[16] = TextureLoader::load2DTexture(std::string("res/terrain/grass/grassAO.tga"), &textureSettings);
		m_Textures[17] = TextureLoader::load2DTexture(std::string("res/terrain/dirt/dirtAO.tga"), &textureSettings);
		m_Textures[18] = TextureLoader::load2DTexture(std::string("res/terrain/branches/branchesAO.tga"), &textureSettings);
		m_Textures[19] = TextureLoader::load2DTexture(std::string("res/terrain/rock/rockAO.tga"), &textureSettings);

		m_Textures[20] = TextureLoader::load2DTexture(std::string("res/terrain/blendMap.tga"), &textureSettings);

		m_Mesh = new Mesh(positions, uvs, normals, tangents, bitangents, indices);
		m_Mesh->LoadData(true);
	}

	Terrain::~Terrain() {
		delete m_Mesh;
	}

	void Terrain::Draw(Shader *shader, RenderPassType pass) const {
		// Texture unit 0 is reserved for the shadowmap
		if (pass == MaterialRequired) {
			int currentTextureUnit = 1;
			// Textures
			m_Textures[0]->bind(currentTextureUnit);
			shader->setUniform("material.texture_albedo1", currentTextureUnit++);
			m_Textures[1]->bind(currentTextureUnit);
			shader->setUniform("material.texture_albedo2", currentTextureUnit++);
			m_Textures[2]->bind(currentTextureUnit);
			shader->setUniform("material.texture_albedo3", currentTextureUnit++);
			m_Textures[3]->bind(currentTextureUnit);
			shader->setUniform("material.texture_albedo4", currentTextureUnit++);

			m_Textures[4]->bind(currentTextureUnit);
			shader->setUniform("material.texture_normal1", currentTextureUnit++);
			m_Textures[5]->bind(currentTextureUnit);
			shader->setUniform("material.texture_normal2", currentTextureUnit++);
			m_Textures[6]->bind(currentTextureUnit);
			shader->setUniform("material.texture_normal3", currentTextureUnit++);
			m_Textures[7]->bind(currentTextureUnit);
			shader->setUniform("material.texture_normal4", currentTextureUnit++);

			m_Textures[8]->bind(currentTextureUnit);
			shader->setUniform("material.texture_roughness1", currentTextureUnit++);
			m_Textures[9]->bind(currentTextureUnit);
			shader->setUniform("material.texture_roughness2", currentTextureUnit++);
			m_Textures[10]->bind(currentTextureUnit);
			shader->setUniform("material.texture_roughness3", currentTextureUnit++);
			m_Textures[11]->bind(currentTextureUnit);
			shader->setUniform("material.texture_roughness4", currentTextureUnit++);

			m_Textures[12]->bind(currentTextureUnit);
			shader->setUniform("material.texture_metallic1", currentTextureUnit++);
			m_Textures[13]->bind(currentTextureUnit);
			shader->setUniform("material.texture_metallic2", currentTextureUnit++);
			m_Textures[14]->bind(currentTextureUnit);
			shader->setUniform("material.texture_metallic3", currentTextureUnit++);
			m_Textures[15]->bind(currentTextureUnit);
			shader->setUniform("material.texture_metallic4", currentTextureUnit++);

			m_Textures[16]->bind(currentTextureUnit);
			shader->setUniform("material.texture_AO1", currentTextureUnit++);
			m_Textures[17]->bind(currentTextureUnit);
			shader->setUniform("material.texture_AO2", currentTextureUnit++);
			m_Textures[18]->bind(currentTextureUnit);
			shader->setUniform("material.texture_AO3", currentTextureUnit++);
			m_Textures[19]->bind(currentTextureUnit);
			shader->setUniform("material.texture_AO4", currentTextureUnit++);

 			m_Textures[20]->bind(currentTextureUnit);
 			shader->setUniform("material.blendmap", currentTextureUnit++);

			// Normal matrix
			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(m_ModelMatrix)));
			shader->setUniform("normalMatrix", normalMatrix);

			// Tiling amount
			shader->setUniform("material.tilingAmount", m_TextureTilingAmount);
		}

		// Only set normal matrix for non shadowmap pass
		shader->setUniform("model", m_ModelMatrix);

		m_GLCache->setDepthTest(true);
		m_GLCache->setDepthFunc(GL_LESS);
		m_GLCache->setBlend(false);
		m_GLCache->setFaceCull(true);
		m_GLCache->setCullFace(GL_BACK);
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
