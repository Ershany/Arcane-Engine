#include "arcpch.h"
#include "Terrain.h"

#include <Arcane/Graphics/Mesh/Mesh.h>
#include <Arcane/Graphics/Mesh/Model.h>
#include <Arcane/Graphics/Renderer/GLCache.h>
#include <Arcane/Graphics/Shader.h>
#include <Arcane/Util/Loaders/AssetManager.h>

namespace Arcane
{
	Terrain::Terrain(glm::vec3 &worldPosition) : m_Position(worldPosition)
	{
		m_GLCache = GLCache::GetInstance();

		m_ModelMatrix = glm::translate(m_ModelMatrix, worldPosition);

		// Height map
		int mapWidth, mapHeight;
		unsigned char *heightMapImage = stbi_load("res/terrain/heightMap.png", &mapWidth, &mapHeight, 0, 1);
		if (mapWidth != mapHeight) {
			ARC_LOG_FATAL("Can't use a heightmap with a different width and height for the terrain");
			return;
		}

		// Terrain information
		m_TextureTilingAmount = 64;
		m_HeightfieldTextureSize = mapWidth;
		m_SideVertexCount = (unsigned int)(mapWidth * 0.25f);
		m_TerrainSizeXZ = 512.0;
		m_TerrainSizeY = 100.0f;
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

				positions.push_back(glm::vec3(positionXZ.x, SampleHeightfieldBilinear(positionXZ.x, positionXZ.y, heightMapImage), positionXZ.y));
				uvs.push_back(glm::vec2((float)x / (float)(m_SideVertexCount - 1), (float)z / (float)(m_SideVertexCount - 1)));
				normals.push_back(CalculateNormal(positionXZ.x, positionXZ.y, heightMapImage));
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
		AssetManager &assetManager = AssetManager::GetInstance();

		TextureSettings srgbTextureSettings;
		srgbTextureSettings.IsSRGB = true;
		
		m_Textures[0] = assetManager.Load2DTextureAsync(std::string("res/terrain/grass/grassAlbedo.tga"), &srgbTextureSettings);
		m_Textures[1] = assetManager.Load2DTextureAsync(std::string("res/terrain/dirt/dirtAlbedo.tga"), &srgbTextureSettings);
		m_Textures[2] = assetManager.Load2DTextureAsync(std::string("res/terrain/branches/branchesAlbedo.tga"), &srgbTextureSettings);
		m_Textures[3] = assetManager.Load2DTextureAsync(std::string("res/terrain/rock/rockAlbedo.tga"), &srgbTextureSettings);

		m_Textures[4] = assetManager.Load2DTextureAsync(std::string("res/terrain/grass/grassNormal.tga"));
		m_Textures[5] = assetManager.Load2DTextureAsync(std::string("res/terrain/dirt/dirtNormal.tga"));
		m_Textures[6] = assetManager.Load2DTextureAsync(std::string("res/terrain/branches/branchesNormal.tga"));
		m_Textures[7] = assetManager.Load2DTextureAsync(std::string("res/terrain/rock/rockNormal.tga"));

		// We do not want these texture treated as one channel so store it as RGB
		TextureSettings textureSettings;
		textureSettings.TextureFormat = GL_RGB;

		m_Textures[8] = assetManager.Load2DTextureAsync(std::string("res/terrain/grass/grassRoughness.tga"), &textureSettings);
		m_Textures[9] = assetManager.Load2DTextureAsync(std::string("res/terrain/dirt/dirtRoughness.tga"), &textureSettings);
		m_Textures[10] = assetManager.Load2DTextureAsync(std::string("res/terrain/branches/branchesRoughness.tga"), &textureSettings);
		m_Textures[11] = assetManager.Load2DTextureAsync(std::string("res/terrain/rock/rockRoughness.tga"), &textureSettings);

		m_Textures[12] = assetManager.Load2DTextureAsync(std::string("res/terrain/grass/grassMetallic.tga"), &textureSettings);
		m_Textures[13] = assetManager.Load2DTextureAsync(std::string("res/terrain/dirt/dirtMetallic.tga"), &textureSettings);
		m_Textures[14] = assetManager.Load2DTextureAsync(std::string("res/terrain/branches/branchesMetallic.tga"), &textureSettings);
		m_Textures[15] = assetManager.Load2DTextureAsync(std::string("res/terrain/rock/rockMetallic.tga"), &textureSettings);

		m_Textures[16] = assetManager.Load2DTextureAsync(std::string("res/terrain/grass/grassAO.tga"), &textureSettings);
		m_Textures[17] = assetManager.Load2DTextureAsync(std::string("res/terrain/dirt/dirtAO.tga"), &textureSettings);
		m_Textures[18] = assetManager.Load2DTextureAsync(std::string("res/terrain/branches/branchesAO.tga"), &textureSettings);
		m_Textures[19] = assetManager.Load2DTextureAsync(std::string("res/terrain/rock/rockAO.tga"), &textureSettings);

		m_Textures[20] = assetManager.Load2DTextureAsync(std::string("res/terrain/blendMap.tga"), &textureSettings);

		m_Mesh = new Mesh(std::move(positions), std::move(uvs), std::move(normals), std::move(tangents), std::move(bitangents), std::move(indices));
		m_Mesh->LoadData(true);
		m_Mesh->GenerateGpuData();
	}

	Terrain::~Terrain() {
		delete m_Mesh;
	}

	void Terrain::Draw(Shader *shader, RenderPassType pass) const {
		// Texture unit 0 is reserved for the directional light shadowmap
		// Texture unit 1 is reserved for the spot light shadowmap
		// Texture unit 2 is reserved for the point light shadowmap
		if (pass == MaterialRequired) {
			int currentTextureUnit = 3;
			// Textures
			m_Textures[0]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_albedo1", currentTextureUnit++);
			m_Textures[1]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_albedo2", currentTextureUnit++);
			m_Textures[2]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_albedo3", currentTextureUnit++);
			m_Textures[3]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_albedo4", currentTextureUnit++);

			m_Textures[4]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_normal1", currentTextureUnit++);
			m_Textures[5]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_normal2", currentTextureUnit++);
			m_Textures[6]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_normal3", currentTextureUnit++);
			m_Textures[7]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_normal4", currentTextureUnit++);

			m_Textures[8]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_roughness1", currentTextureUnit++);
			m_Textures[9]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_roughness2", currentTextureUnit++);
			m_Textures[10]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_roughness3", currentTextureUnit++);
			m_Textures[11]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_roughness4", currentTextureUnit++);

			m_Textures[12]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_metallic1", currentTextureUnit++);
			m_Textures[13]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_metallic2", currentTextureUnit++);
			m_Textures[14]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_metallic3", currentTextureUnit++);
			m_Textures[15]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_metallic4", currentTextureUnit++);

			m_Textures[16]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_AO1", currentTextureUnit++);
			m_Textures[17]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_AO2", currentTextureUnit++);
			m_Textures[18]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_AO3", currentTextureUnit++);
			m_Textures[19]->Bind(currentTextureUnit);
			shader->SetUniform("material.texture_AO4", currentTextureUnit++);

 			m_Textures[20]->Bind(currentTextureUnit);
 			shader->SetUniform("material.blendmap", currentTextureUnit++);

			// Normal matrix
			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(m_ModelMatrix)));
			shader->SetUniform("normalMatrix", normalMatrix);

			// Tiling amount
			shader->SetUniform("material.tilingAmount", m_TextureTilingAmount);
		}

		// Only set normal matrix for non shadowmap pass
		shader->SetUniform("model", m_ModelMatrix);

		m_GLCache->SetDepthTest(true);
		m_GLCache->SetDepthFunc(GL_LESS);
		m_GLCache->SetBlend(false);
		m_GLCache->SetFaceCull(true);
		m_GLCache->SetCullFace(GL_BACK);
		m_Mesh->Draw();
	}

	// Bilinear filtering for the terrain's normal
	glm::vec3 Terrain::CalculateNormal(float worldPosX, float worldPosZ, unsigned char *heightMapData) {
		float heightR = SampleHeightfieldNearest(worldPosX + m_SpaceBetweenVertices * 2, worldPosZ                         , heightMapData);
		float heightL = SampleHeightfieldNearest(worldPosX - m_SpaceBetweenVertices * 2, worldPosZ                         , heightMapData);
		float heightU = SampleHeightfieldNearest(worldPosX                         , worldPosZ + m_SpaceBetweenVertices * 2, heightMapData);
		float heightD = SampleHeightfieldNearest(worldPosX                         , worldPosZ - m_SpaceBetweenVertices * 2, heightMapData);
		
		glm::vec3 normal(heightL - heightR, 2.0f, heightD - heightU);
		normal = glm::normalize(normal);

		return normal;
	}

	float Terrain::SampleHeightfieldBilinear(float worldPosX, float worldPosZ, unsigned char *heightMapData) {
		// Calculate weights
		glm::vec2 weightsXZ = glm::vec2(worldPosX / m_SpaceBetweenVertices, worldPosZ / m_SpaceBetweenVertices);
		float xFrac = weightsXZ.x - (int)weightsXZ.x;
		float zFrac = weightsXZ.y - (int)weightsXZ.y;

		// Get the values that should be lerped between
		float topLeft = SampleHeightfieldNearest(worldPosX, worldPosZ, heightMapData);
		float topRight = SampleHeightfieldNearest(worldPosX + m_SpaceBetweenVertices, worldPosZ, heightMapData);
		float bottomLeft = SampleHeightfieldNearest(worldPosX, worldPosZ + m_SpaceBetweenVertices, heightMapData);
		float bottomRight = SampleHeightfieldNearest(worldPosX + m_SpaceBetweenVertices, worldPosZ + m_SpaceBetweenVertices, heightMapData);

		// Do the bilinear filtering
		float terrainHeight = glm::mix(glm::mix(topLeft, topRight, xFrac), glm::mix(bottomLeft, bottomRight, xFrac), zFrac);
		return terrainHeight;
	}

	float Terrain::SampleHeightfieldNearest(float worldPosX, float worldPosZ, unsigned char *heightMapData) {
		// Get the xz coordinates of the index after cutting off the decimal
		glm::vec2 terrainXZ = glm::vec2(Clamp(worldPosX * m_TerrainToHeightfieldTextureConversion, 0.0f, (float)m_HeightfieldTextureSize - 0.0f), Clamp(worldPosZ * m_TerrainToHeightfieldTextureConversion, 0.0f, (float)m_HeightfieldTextureSize - 1.0f));

		// Normalize height to [0, 1] then multiply it by the terrain's Y scale
		return (heightMapData[(unsigned int)terrainXZ.x + ((unsigned int)terrainXZ.y * m_HeightfieldTextureSize)] / 255.0f) * m_TerrainSizeY;
	}

	float Terrain::Clamp(float n, float lower, float upper) {
		return std::max<float>(lower, std::min<float>(n, upper));
	}
}
