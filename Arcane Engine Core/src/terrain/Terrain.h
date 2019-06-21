#pragma once

#include <graphics/mesh/Mesh.h>
#include <graphics/mesh/Model.h>
#include <graphics/renderer/GLCache.h>
#include <graphics/Shader.h>
#include <utils/loaders/TextureLoader.h>

namespace arcane {

	class Terrain {
	public:
		Terrain(glm::vec3 &worldPosition);
		~Terrain();

		void Draw(Shader *shader, RenderPassType pass) const;

		inline const glm::vec3& getPosition() const { return m_Position; }
	private:
		glm::vec3 calculateNormal(float worldPosX, float worldPosZ, unsigned char *heightMapData);

		float sampleHeightfieldBilinear(float worldPosX, float worldPosZ, unsigned char *heightMapData);
		float sampleHeightfieldNearest(float worldPosX, float worldPosZ, unsigned char *heightMapData);

		float Terrain::clamp(float n, float lower, float upper);
	private:
		GLCache *m_GLCache;

		// Tweakable Terrain Variables
		float m_TextureTilingAmount;
		float m_TerrainSizeXZ, m_TerrainSizeY;
		unsigned int m_SideVertexCount;

		// Non-Tweakable Terrain Varialbes
		float m_SpaceBetweenVertices;
		float m_TerrainToHeightfieldTextureConversion;
		unsigned int m_HeightfieldTextureSize;

		glm::mat4 m_ModelMatrix;
		glm::vec3 m_Position;
		Mesh *m_Mesh;
		std::array<Texture*, 21> m_Textures; // Represents all the textures supported by the terrain's texure splatting (rgba and the default value)
	};

}
