#pragma once
#ifndef TERRAIN_H
#define TERRAIN_H

#ifndef RENDERPASSTYPE_H
#include <Arcane/Graphics/Renderer/Renderpass/RenderPassType.h>
#endif

namespace Arcane
{
	class Shader;
	class Mesh;
	class GLCache;

	class Terrain
	{
	public:
		Terrain();
		~Terrain();

		void LoadTerrainFromTexture(const char *texturePath);
		void LoadTerrainFromFile(const char *filePath);

		void Draw(Shader *shader, RenderPassType pass) const;

		inline void SetPosition(glm::vec3 &pos) { m_Position = pos; m_ModelMatrix = glm::translate(glm::mat4(1.0f), pos); }
		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline bool IsLoaded() { return m_Mesh != nullptr; }
	private:
		glm::vec3 CalculateNormal(float worldPosX, float worldPosZ, unsigned char *heightMapData);

		float SampleHeightfieldBilinear(float worldPosX, float worldPosZ, unsigned char *heightMapData);
		float SampleHeightfieldNearest(float worldPosX, float worldPosZ, unsigned char *heightMapData);

		float Clamp(float n, float lower, float upper);
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
		Mesh *m_Mesh = nullptr;
		std::array<Texture*, 21> m_Textures; // Represents all the textures supported by the terrain's texure splatting (rgba and the default value)
	};
}
#endif
